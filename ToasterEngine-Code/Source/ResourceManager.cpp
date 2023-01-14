#include "ResourceManager.h"

#include "Application.h"
#include "../External/ImGui/imgui.h"

//Importers
//#include "IM_FileSystem.h"
#include "ModuleTexture.h"
#include "ModuleMesh3D.h"
#include "ModuleModelImporter.h"
#include "ModuleShaderImporter.h"

#include "Globals.h"

#include "R_Texture.h"
#include "R_Mesh.h"
#include "R_Shader.h"
#include "R_Material.h"
#include "R_Animation.h"

#include "ModuleWindow.h"
//#include "MO_MonoManager.h"

#include "DtTimer.h"

M_ResourceManager::M_ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

M_ResourceManager::~M_ResourceManager()
{
}

bool M_ResourceManager::Init()
{
	return true;
}

bool M_ResourceManager::Start()
{
	/*assetsRoot.lastModTime = App->moduleFileSystem->GetLastModTime(assetsRoot.importPath.c_str());
	meshesLibraryRoot.lastModTime = App->moduleFileSystem->GetLastModTime(meshesLibraryRoot.importPath.c_str());
	animationsLibraryRoot.lastModTime = App->moduleFileSystem->GetLastModTime(animationsLibraryRoot.importPath.c_str());

	NeedsDirsUpdate(assetsRoot);*/

	return true;
}

update_status M_ResourceManager::PreUpdate(float dt)
{
	if (DETime::state == GameState::PLAY || DETime::state == GameState::PAUSE)
		return update_status::UPDATE_CONTINUE;

	fileCheckTime += dt;
	if (fileCheckTime >= fileUpdateDelay && (SDL_GetWindowFlags(app->window->window) & SDL_WindowFlags::SDL_WINDOW_MOUSE_FOCUS || SDL_GetWindowFlags(app->window->window) & SDL_WindowFlags::SDL_WINDOW_INPUT_FOCUS))
	{
		//NeedsDirsUpdate(assetsRoot);
	}

	return update_status::UPDATE_CONTINUE;
}

bool M_ResourceManager::CleanUp()
{
	return true;
}

int M_ResourceManager::GenerateNewUID()
{
	return RandomIntValue();
}

void M_ResourceManager::ZeroReferenceCleanUp()
{
	std::vector<Resource*> toDelete;
	for (auto i = resources.begin(); i != resources.end(); i++)
	{
		if (i->second->GetReferenceCount() <= 0)
			toDelete.push_back(i->second);
	}

	for (size_t j = 0; j < toDelete.size(); ++j)
	{
		ReleaseResource(toDelete[j]->GetUID());
	}
	toDelete.clear();
}

void M_ResourceManager::ReleaseResource(int uid)
{
	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it == resources.end())
		return;

	Resource* res = (*it).second;
	(*it).second->UnloadFromMemory();
	resources.erase((*it).second->GetUID());
	delete res;
}

int M_ResourceManager::ImportFile(const char* assetsFile, Resource::Type type)
{
	char* fileBuffer = nullptr;
	unsigned int size = app->importer->FileToBuffer(assetsFile, &fileBuffer);

	//static_assert(static_cast<int>(Resource::Type::UNKNOWN) == 12, "Update all switches with new type");

	Resource* resource;

	switch (type)
	{
	case Resource::Type::TEXTURE: app->textures->Import(fileBuffer, size, resource); break;
	case Resource::Type::MODEL: ModelImporter::Import(fileBuffer, size, resource); break;
		//case Resource::Type::MESH: MeshLoader::BufferToMeshes(fileBuffer, size, resource); break;
	case Resource::Type::SHADER: app->shaders->Import(fileBuffer, size, dynamic_cast<ResourceShader*>(resource), assetsFile); break;
	case Resource::Type::MATERIAL: app->materials->Import(fileBuffer, size, resource);/* app->importer->Save(resource->GetLibraryPath(), fileBuffer, size);*/ break;
	case Resource::Type::ANIMATION: resource = CreateNewResource(assetsFile, 0, type); /* app->importer->Save(resource->GetLibraryPath(), fileBuffer, size); */break;
	case Resource::Type::UNKNOWN: LOG(" Unknown File error"); return 0;
	}

	RELEASE_ARRAY(fileBuffer);

	return resource->GetUID();;
}

Resource* M_ResourceManager::CreateNewResource(const char* assetsFile, uint uid, Resource::Type type)
{
	Resource* ret = nullptr;

	if (uid == 0)
		uid = GenerateNewUID();

	//static_assert(static_cast<int>(Resource::Type::UNKNOWN) == 12, "Update all switches with new type");

	switch (type)
	{
	case Resource::Type::TEXTURE: ret = (Resource*) new ResourceTexture(uid); break;
	case Resource::Type::MODEL: ret = new Resource(uid, Resource::Type::MODEL); break;
	case Resource::Type::MESH: ret = (Resource*) new ResourceMesh(uid); break;
	case Resource::Type::SHADER: ret = (Resource*) new ResourceShader(uid); break;
	case Resource::Type::MATERIAL:
		ret = (Resource*) new ResourceMaterial(uid); break;
	case Resource::Type::ANIMATION: ret = (Resource*) new ResourceAnimation(uid); break;
	}

	if (ret != nullptr)
	{
		resources[uid] = ret;
		ret->SetAssetsPath(assetsFile);
		//ret->SetLibraryPath(GenLibraryPath(ret->GetUID(), type).c_str());
		ret->IncreaseReferenceCount();
		std::string name;
		name = app->importer->GetFileName(assetsFile, false);
		sprintf_s(ret->name, name.c_str());
	}

	return ret;
}

Resource* M_ResourceManager::GetResourceFromUID(int uid)
{
	//Find if the resource is already loaded
	if (uid <= -1)
		return nullptr;

	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
	{
		return it->second;
	}

	return nullptr;
}

void M_ResourceManager::LoadResource(int uid)
{
	Resource* res = nullptr;

	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it == resources.end())
		return;

	res = it->second;
	res->IncreaseReferenceCount();

	res->LoadToMemory();
}

void M_ResourceManager::UnloadResource(int uid, bool releaseAtZero)
{
	Resource* res = nullptr;

	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it == resources.end())
		return;

	res = it->second;
	res->DecreaseReferenceCount();

	if (releaseAtZero == true && res->GetReferenceCount() <= 0)
		ReleaseResource(res->GetUID());
}

bool M_ResourceManager::IsResourceLoaded(int uid)
{
	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
		return true;

	return false;
}

Resource::Type M_ResourceManager::GetTypeFromAssetExtension(const char* assetFile) const
{
	std::string ext(assetFile);
	ext = ext.substr(ext.find_last_of('.') + 1);

	for (size_t i = 0; i < ext.length(); i++)
	{
		ext[i] = std::tolower(ext[i]);
	}

	if (ext == "fbx" || ext == "FBX" || ext == "dae")
		return Resource::Type::MODEL;
	if (ext == "tga" || ext == "png" || ext == "jpg" || ext == "dds")
		return Resource::Type::TEXTURE;
	if (ext == "des")
		return Resource::Type::SHADER;
	if (ext == "mat")
		return Resource::Type::MATERIAL;
	if (ext == "anim")
		return Resource::Type::ANIMATION;

	return Resource::Type::UNKNOWN;
}