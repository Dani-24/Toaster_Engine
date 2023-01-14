#include "C_Mesh.h"
#include "R_Mesh.h"

#include "R_Material.h"
#include "R_Shader.h"
#include "R_Texture.h"

#include "Application.h"

#include "ModuleRenderer3D.h"
#include "ResourceManager.h"
//#include "MO_Scene.h"

#include "ModuleImporter.h"

#include "GameObject.h"

#include "C_Texture.h"

#include "C_Transform.h"
#include "C_Camera.h"

#include "../External/ImGui/imgui.h"

#include "OpenGL.h"

//#include "DEJsonSupport.h"

#include "../External/MathGeoLib/include/Geometry/Frustum.h"
#include "../External/MathGeoLib/include/Geometry/Plane.h"


C_Mesh::C_Mesh(GameObject* _gm) : Component(_gm), _mesh(nullptr), normalMap(nullptr), specularMap(nullptr), bumpDepth(0.0f),
faceNormals(false), vertexNormals(false), showAABB(false), showOBB(false), drawDebugVertices(false),
calculatedBonesThisFrame(false), boneTransforms()
{
	name = "Mesh Renderer";
	gameObjectTransform = dynamic_cast<C_Transform*>(gameObject->GetComponent(Component::TYPE::TRANSFORM));
	alternColor = float3::one;
}

C_Mesh::~C_Mesh()
{
	rootBone = nullptr;
	bonesMap.clear();
	boneTransforms.clear();

	if (_mesh != nullptr)
	{
		app->scene->totalTris -= _mesh->indices_count / 3;
		app->resourceManager->UnloadResource(_mesh->GetUID());
		_mesh = nullptr;
	}

	if (normalMap != nullptr)
	{
		app->resourceManager->UnloadResource(normalMap->GetUID());
		normalMap = nullptr;
	}

	if (specularMap != nullptr)
	{
		app->resourceManager->UnloadResource(specularMap->GetUID());
		specularMap = nullptr;
	}

	gameObjectTransform = nullptr;
}

void C_Mesh::Update()
{
	calculatedBonesThisFrame = false;
	boneTransforms.clear();

	app->editor->renderQueue.push_back(this);

	/*if (showAABB == true)
	{
		float3 points[8];
		globalAABB.GetCornerPoints(points);

		app->renderer3D->DrawBox(points, float3(0.2f, 1.f, 0.101f));
	}

	if (showOBB == true)
	{

		float3 points[8];
		globalOBB.GetCornerPoints(points);

		app->renderer3D->DrawBox(points);
	}*/

}

void C_Mesh::RenderMesh(bool rTex)
{
	if (_mesh == nullptr)
		return;

	C_Transform* transform = gameObject->transform;

	//TODO IMPORTANT: Optimize this, save this pointer or something
	C_Material* material = dynamic_cast<C_Material*>(gameObject->GetComponent(Component::TYPE::MATERIAL));
	GLuint id = 0;

	if (material != nullptr)
	{
		if (material->IsActive())
			id = material->GetTextureID();
	}

	TryCalculateBones();

	if (drawDebugVertices)
		DrawDebugVertices();

	_mesh->RenderMesh(id, alternColor, rTex, (material && material->material != nullptr) ? material->material : app->scene->defaultMaterial, transform, normalMap, specularMap, bumpDepth);

	if (vertexNormals || faceNormals)
		_mesh->RenderMeshDebug(&vertexNormals, &faceNormals, transform->GetGlobalTransposed());

}

bool C_Mesh::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		if (_mesh != nullptr)
		{
			//ImGui::Image((ImTextureID)_mesh->textureID, ImVec2(128, 128));
			ImGui::Text("Vertices: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", _mesh->vertices_count);
			ImGui::Text("Indices: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", _mesh->indices_count);

			ImGui::Spacing();
			ImGui::Text("Path: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", _mesh->GetAssetPath());
		}

		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Normal map");
		if (normalMap != nullptr)
			ImGui::Image((ImTextureID)normalMap->textureID, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));

		else
			ImGui::Image((ImTextureID)app->renderer3D->defaultNormalMap, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));

		//if (ImGui::BeginDragDropTarget())
		//{
		//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
		//	{
		//		//Drop asset from Asset window to scene window
		//		std::string* metaFileDrop = (std::string*)payload->Data;

		//		if (normalMap != nullptr)
		//			app->resourceManager->UnloadResource(normalMap->GetUID());

		//		std::string libraryName = app->resourceManager->LibraryFromMeta(metaFileDrop->c_str());

		//		normalMap = dynamic_cast<ResourceTexture*>(app->resourceManager->RequestResource(EngineExternal->moduleResources->GetMetaUID(metaFileDrop->c_str()), libraryName.c_str()));
		//		LOG("File %s loaded to scene", (*metaFileDrop).c_str());
		//	}
		//	ImGui::EndDragDropTarget();
		//}

		if (normalMap != nullptr)
		{
			char name[32];
			sprintf_s(name, "Remove normal map: %d", normalMap->GetUID());
			if (ImGui::Button(name))
			{
				app->resourceManager->UnloadResource(normalMap->GetUID());
				normalMap = nullptr;
			}
		}

		//Specular map
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Specular map");
		if (specularMap != nullptr)
			ImGui::Image((ImTextureID)specularMap->textureID, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));

		else
			ImGui::Image((ImTextureID)app->renderer3D->defaultSpecularMap, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));

		//if (ImGui::BeginDragDropTarget())
		//{
		//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
		//	{
		//		//Drop asset from Asset window to scene window
		//		std::string* metaFileDrop = (std::string*)payload->Data;

		//		if (specularMap != nullptr)
		//			app->resourceManager->UnloadResource(specularMap->GetUID());

		//		std::string libraryName = app->resourceManager->LibraryFromMeta(metaFileDrop->c_str());

		//		specularMap = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(EngineExternal->moduleResources->GetMetaUID(metaFileDrop->c_str()), libraryName.c_str()));
		//		LOG("File %s loaded to scene", (*metaFileDrop).c_str());
		//	}
		//	ImGui::EndDragDropTarget();
		//}

		if (specularMap != nullptr)
		{
			char name[32];
			sprintf_s(name, "Remove specular map: %d", specularMap->GetUID());
			if (ImGui::Button(name))
			{
				app->resourceManager->UnloadResource(specularMap->GetUID());
				specularMap = nullptr;
			}
		}

		ImGui::NewLine();
		ImGui::DragFloat("Bump depth", &bumpDepth, 0.001f, -1, 1);
		ImGui::NewLine();
		ImGui::Separator();

		ImGui::Checkbox("Vertex Normals", &vertexNormals);
		ImGui::SameLine();
		ImGui::Checkbox("Show AABB", &showAABB);
		ImGui::Checkbox("Face Normals", &faceNormals);
		ImGui::SameLine();
		ImGui::Checkbox("Show OBB", &showOBB);
		ImGui::Checkbox("Draw Vertices", &drawDebugVertices);

		return true;
	}
	return false;
}

void C_Mesh::SetRootBone(GameObject* _rootBone)
{
	if (_rootBone == nullptr) {
		LOG("Trying to assign null root bone");
		return;
	}

	rootBone = _rootBone;

	//Get all the bones
	GetBoneMapping();

	//Set bone Transforms array size using original bones transform array size
	_mesh->boneTransforms.resize(_mesh->bonesOffsets.size());

	if (bonesMap.size() != _mesh->bonesMap.size())
	{
		for (size_t i = 0; i < _mesh->boneTransforms.size(); i++)
		{
			_mesh->boneTransforms[i] = float4x4::identity;
		}
	}
}

void C_Mesh::SetRenderMesh(ResourceMesh* mesh)
{
	_mesh = mesh;

	if (mesh == nullptr)
		return;

	app->scene->totalTris += _mesh->indices_count / 3;

	globalOBB = _mesh->localAABB;
	globalOBB.Transform(gameObject->transform->globalTransform);

	// Generate global AABB
	globalAABB.SetNegativeInfinity();
	globalAABB.Enclose(globalOBB);

	_mesh->generalWireframe = &app->renderer3D->wireframe;
}

ResourceMesh* C_Mesh::GetRenderMesh()
{
	return _mesh;
}

float4x4 C_Mesh::CalculateDeltaMatrix(float4x4 globalMat, float4x4 invertMat)
{
	return invertMat * globalMat;
}

void C_Mesh::GetBoneMapping()
{
	std::vector<GameObject*> gameObjects;
	rootBone->CollectChilds(gameObjects);

	bonesMap.resize(_mesh->bonesMap.size());
	for (uint i = 0; i < gameObjects.size(); ++i)
	{
		std::map<std::string, uint>::iterator it = _mesh->bonesMap.find(gameObjects[i]->name);
		if (it != _mesh->bonesMap.end())
		{
			bonesMap[it->second] = dynamic_cast<C_Transform*>(gameObjects[i]->GetComponent(Component::TYPE::TRANSFORM));
		}
	}
}

void C_Mesh::DrawDebugVertices()
{
	if (_mesh->boneTransforms.size() > 0)
	{
		for (uint v = 0; v < _mesh->vertices_count; ++v)
		{
			float3 vertex;
			vertex.x = _mesh->vertices[v * VERTEX_ATTRIBUTES];
			vertex.y = _mesh->vertices[v * VERTEX_ATTRIBUTES + 1];
			vertex.z = _mesh->vertices[v * VERTEX_ATTRIBUTES + 2];

			//For each set of 4 bones for bertex
			for (uint b = 0; b < 4; ++b)
			{
				//Get bone identificator and weights from arrays
				int bone_ID = _mesh->vertices[v * VERTEX_ATTRIBUTES + BONES_ID_OFFSET + b];
				float boneWeight = _mesh->vertices[v * VERTEX_ATTRIBUTES + WEIGHTS_OFFSET + b];

				//Meaning boneWeight will be 0
				if (bone_ID == -1)
					continue;

				//Transforming original mesh vertex with bone transformation matrix
				float3 vertexTransform;
				vertexTransform.x = _mesh->vertices[v * VERTEX_ATTRIBUTES];
				vertexTransform.y = _mesh->vertices[v * VERTEX_ATTRIBUTES + 1];
				vertexTransform.z = _mesh->vertices[v * VERTEX_ATTRIBUTES + 2];
				//float3 Inc = _mesh->boneTransforms[bone_ID].TransformPos(vertexTransform);

				float4 Inc = _mesh->boneTransforms[bone_ID].Transposed().Mul(float4(vertexTransform, 1.0));

				vertex.x += Inc.x * boneWeight;
				vertex.y += Inc.y * boneWeight;
				vertex.z += Inc.z * boneWeight;
			}

			glPushMatrix();
			glMultMatrixf(gameObject->transform->globalTransform.Transposed().ptr());
			glPointSize(4.0f);
			glBegin(GL_POINTS);
			glVertex3fv(vertex.ptr());
			glEnd();
			glPointSize(1.0f);
			glPopMatrix();
		}
	}
}

void C_Mesh::TryCalculateBones()
{

	if (rootBone == nullptr)
		return;

	//Mesh array with transform matrix of each bone
	if (calculatedBonesThisFrame == false)
	{
		//float4x4 invertedMatrix = dynamic_cast<C_Transform*>(gameObject->GetComponent(Component::TYPE::TRANSFORM))->globalTransform.Inverted();
		float4x4 invertedMatrix = gameObjectTransform->globalTransform.Inverted();

		boneTransforms.reserve(_mesh->bonesMap.size());

		//Get each bone
		for (int i = 0; i < _mesh->bonesMap.size(); ++i)
		{
			C_Transform* bone = bonesMap[i];

			if (bone != nullptr)
			{
				//Calcule of Delta Matrix
				float4x4 Delta = CalculateDeltaMatrix(bone->globalTransform, invertedMatrix);
				Delta = Delta * _mesh->bonesOffsets[i];

				//Storage of Delta Matrix (Transformation applied to each bone)
				//_mesh->boneTransforms[i] = Delta.Transposed();
				boneTransforms.push_back(Delta.Transposed());
			}
		}
		calculatedBonesThisFrame = true;
	}

	if (_mesh->boneTransforms.size() != boneTransforms.size())
		return;

	for (int i = 0; i < _mesh->bonesMap.size(); ++i)
	{
		_mesh->boneTransforms[i] = boneTransforms[i];
	}

}

void C_Mesh::DrawAABB() {
	if (this->gameObject != app->editor->root) {
		float3 corners[8];
		float3 frustum_corners[8];

		AABB aabb = globalAABB;

		// Get Frustum corners
		corners[0] = aabb.CornerPoint(0);
		corners[1] = aabb.CornerPoint(2);
		corners[2] = aabb.CornerPoint(4);
		corners[3] = aabb.CornerPoint(6);
		corners[4] = aabb.CornerPoint(1);
		corners[5] = aabb.CornerPoint(3);
		corners[6] = aabb.CornerPoint(5);
		corners[7] = aabb.CornerPoint(7);

		std::vector<float3> frustum_lines;

		frustum_lines.push_back(corners[0]);
		frustum_lines.push_back(corners[1]);
		frustum_lines.push_back(corners[0]);
		frustum_lines.push_back(corners[2]);
		frustum_lines.push_back(corners[1]);
		frustum_lines.push_back(corners[3]);
		frustum_lines.push_back(corners[2]);
		frustum_lines.push_back(corners[3]);

		frustum_lines.push_back(corners[4]);
		frustum_lines.push_back(corners[5]);
		frustum_lines.push_back(corners[4]);
		frustum_lines.push_back(corners[6]);
		frustum_lines.push_back(corners[5]);
		frustum_lines.push_back(corners[7]);
		frustum_lines.push_back(corners[6]);
		frustum_lines.push_back(corners[7]);

		frustum_lines.push_back(corners[0]);
		frustum_lines.push_back(corners[4]);
		frustum_lines.push_back(corners[1]);
		frustum_lines.push_back(corners[5]);
		frustum_lines.push_back(corners[2]);
		frustum_lines.push_back(corners[6]);
		frustum_lines.push_back(corners[3]);
		frustum_lines.push_back(corners[7]);

		// Add Lines to the DrawLines queue
		for (int i = 0; i < frustum_lines.size(); i++) {
			if (this->gameObject->GetComponent(Component::TYPE::CAMERA) == nullptr) {
				app->scene->AddLines(frustum_lines[i], Red);
			}
			else {
				app->scene->AddLines(frustum_lines[i], Green);
			}
		}
	}
}

AABB C_Mesh::GetAABB() {
	return globalAABB;
}