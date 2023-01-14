#include "ModuleModelImporter.h"

#include "GameObject.h"

#include "../External/Assimp/include/cimport.h"
#include "../External/Assimp/include/scene.h"
#include "../External/Assimp/include/postprocess.h"
#include "../External/Assimp/include/cfileio.h"

#include "ResourceManager.h"

#include "ModuleImporter.h"

#include "ModuleMesh3D.h"
#include "ModuleAnimation.h"

#include "R_Mesh.h"
#include "R_Texture.h"
#include "R_Animation.h"
#include "Resource.h"

#include "Application.h"

void ModelImporter::Import(char* buffer, int bSize, Resource* res)
{
	const aiScene* scene = aiImportFileFromMemory(buffer, bSize, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	if (scene != nullptr && scene->HasMeshes())
	{
		std::vector<ResourceMesh*> meshesOnModel;
		std::vector<ResourceTexture*> texturesOnModel;
		std::vector<ResourceAnimation*> animationsOnModel;

		//This should not be here
		if (scene->HasMaterials())
		{
			//Needs to be moved to another place
			std::string generalPath(res->GetAssetPath());
			generalPath = generalPath.substr(0, generalPath.find_last_of("/\\") + 1);
			for (size_t k = 0; k < scene->mNumMaterials; k++)
			{
				aiMaterial* material = scene->mMaterials[k];
				uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);

				if (numTextures > 0)
				{
					aiString path;
					material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

					/*std::string localPath = generalPath;
					localPath = localPath.substr(0, localPath.find_last_of('/') + 1);
					localPath += FileSystem::NormalizePath(path.C_Str());

					std::string libraryPath = EngineExternal->moduleResources->GetMetaPath(localPath.c_str());
					uint UID = EngineExternal->moduleResources->GetMetaUID(libraryPath.c_str());
					libraryPath = EngineExternal->moduleResources->LibraryFromMeta(libraryPath.c_str());*/

					ResourceTexture* texture = dynamic_cast<ResourceTexture*>(app->resourceManager->RequestResource(UID, libraryPath.c_str()));

					texturesOnModel.push_back(texture);

					path.Clear();
				}
				else
				{
					texturesOnModel.push_back(nullptr); //Empty texture
				}
			}
		}

		//Load mesh uid from meta file
		std::vector<uint> meshUIDs;
		/*if (FileSystem::Exists(EngineExternal->moduleResources->GetMetaPath(res->GetAssetPath()).c_str()))
			GetMeshesFromMeta(res->GetAssetPath(), meshUIDs);*/

		if (scene->HasMeshes())
		{
			for (unsigned int i = 0; i < scene->mNumMeshes; i++)
			{
				if (meshUIDs.size() != 0)
					meshesOnModel.push_back(app->mesh3d->LoadMesh(scene->mMeshes[i], meshUIDs[i]));
				else
					meshesOnModel.push_back(app->mesh3d->LoadMesh(scene->mMeshes[i]));
			}
		}

		//SaveMeshesToMeta(res->GetAssetPath(), meshesOnModel);
		//EngineExternal->moduleResources->UpdateMeshesDisplay();

		std::vector<uint> animationsUIDs;
		/*if (FileSystem::Exists(EngineExternal->moduleResources->GetMetaPath(res->GetAssetPath()).c_str()))
			GetAnimationsFromMeta(res->GetAssetPath(), animationsUIDs);*/

		if (scene->HasAnimations())
		{
			for (unsigned int i = 0; i < scene->mNumAnimations; i++)
			{
				if (animationsUIDs.size() != 0)
					animationsOnModel.push_back(app->anim3d->ImportAnimation(scene->mAnimations[i], animationsUIDs[i]));
				else
					animationsOnModel.push_back(app->anim3d->ImportAnimation(scene->mAnimations[i]));
			}
		}

		//SaveAnimationsToMeta(res->GetAssetPath(), animationsOnModel);
		//EngineExternal->moduleResources->UpdateAnimationsDisplay();

		//Save custom format model
		GameObject* root = new GameObject("First model GO", nullptr);

		std::string name = "";
		//FileSystem::GetFileName(res->GetAssetPath(), name, false);

		app->mesh3d->NodeToGameObject(scene->mMeshes, texturesOnModel, meshesOnModel, scene->mRootNode, root, name.c_str());

		//SaveModelCustom(root->children[0], res->GetLibraryPath());
		for (size_t i = 0; i < meshesOnModel.size(); i++)
		{
			meshesOnModel[i]->UnloadFromMemory();
		}

		delete root;
		root = nullptr;

		for (size_t i = 0; i < meshesOnModel.size(); i++) {
			meshesOnModel[i] = nullptr;
		}
		meshesOnModel.clear();

		for (size_t i = 0; i < texturesOnModel.size(); i++) {
			texturesOnModel[i] = nullptr;
		}
		texturesOnModel.clear();

		/*for (size_t i = 0; i < animationsOnModel.size(); i++)
		{
			EngineExternal->moduleResources->UnloadResource(animationsOnModel[i]->GetUID());
		}*/
		animationsOnModel.clear();
		meshUIDs.clear();
		animationsUIDs.clear();

		aiReleaseImport(scene);
		scene = nullptr;
	}
	else
		LOG("Error loading aiScene");
}