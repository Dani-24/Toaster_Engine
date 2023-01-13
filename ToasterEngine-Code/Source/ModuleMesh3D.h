#pragma once

#include "Module.h"

#include <vector>
#include "../External/MathGeoLib/include/Math/Quat.h"

class ResourceMesh;
class aiNode;
class aiMesh;
class GameObject;
class ResourceTexture;
typedef unsigned int uint;

class ModuleMesh3D : public Module
{
public:
	ModuleMesh3D(Application* app, bool start_enabled = true);
	~ModuleMesh3D();

	void NodeToGameObject(aiMesh** meshArray, std::vector<ResourceTexture*>& sceneTextures, std::vector<ResourceMesh*>& _sceneMeshes, aiNode* node, GameObject* gmParent, const char* holderName);
	ResourceMesh* LoadMesh(aiMesh* importedMesh, uint oldUID = 0);

	void PopulateTransform(GameObject* child, float3 position, Quat rotationQuat, float3 size);
};