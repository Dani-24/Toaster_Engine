#pragma once
#include "Component.h"
#include <map>
#include "../External/MathGeoLib/include/Geometry/AABB.h"
#include "../External/MathGeoLib/include/Geometry/OBB.h"

class ResourceMesh;
class ResourceTexture;
class C_Transform;

class C_Mesh : public Component
{
public:
	C_Mesh(GameObject* _gm);
	virtual ~C_Mesh();

	void Update() override;

	void RenderMesh(bool rTex = false);

	bool OnEditor() override;

	void SetRootBone(GameObject* _rootBone);
	void SetRenderMesh(ResourceMesh* mesh);
	ResourceMesh* GetRenderMesh();
	float4x4 CalculateDeltaMatrix(float4x4 globalMat, float4x4 invertMat);
	void GetBoneMapping();
	void DrawDebugVertices();
	void TryCalculateBones();
	OBB globalOBB;
	AABB globalAABB;
	bool faceNormals, vertexNormals, showAABB, showOBB;

	float3 alternColor;

	void DrawAABB();
	AABB GetAABB();

	GameObject* rootBone = nullptr;

private:
	ResourceMesh* _mesh = nullptr;
	ResourceTexture* normalMap = nullptr;
	ResourceTexture* specularMap = nullptr;

	float bumpDepth = 0.0f;

	bool drawDebugVertices;
	C_Transform* gameObjectTransform = nullptr;
	std::vector<C_Transform*> bonesMap;
	std::vector<float4x4> boneTransforms;
	bool calculatedBonesThisFrame;
};