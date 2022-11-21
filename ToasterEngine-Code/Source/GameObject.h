#pragma once

#include "Globals.h"
#include "ModuleMesh3D.h"
#include "ModuleTexture.h"

#include <math.h>
#include "../External/MathGeoLib/include/MathGeoLib.h"
#include "../External/ImGui/imgui.h"

struct Transform {
	float3 position = float3(0.0f, 0.0f, 0.0f),
		rotation = float3(0.0f, 0.0f, 0.0f),
		scale = float3(0.0f, 0.0f, 0.0f);
};

class GameObject
{
public:

	GameObject(std::string name, GameObject* parent);
	~GameObject();

	void DeleteThisGameObject();

	// Hierarchy
	void SetParent(GameObject* parent);
	GameObject* GetParent() { return parent; }

	void AddChild(GameObject* child);
	void DeleteChild(GameObject* child);

	// Vars
	std::string GetName() { return name; }
	uint GetID() { return ID; }

	// ImGUI
	void OnEditor();

private:

	uint ID;
	GameObject* parent;
	std::string name;

public:

	std::vector<GameObject*> childs;

	bool pendindToDelete = false;

	// TRANSFORM
public:

	void SetPos(float3 pos);
	void SetRot(float3 rot);
	void SetScale(float3 scale);

	float3 GetPos() { return GO_trans.position; }
	float3 GetRot() { return GO_trans.rotation; }
	float3 GetScale() { return GO_trans.scale; }

	void SetTransform(float3 pos, float3 rot, float3 scale);

	void Translate(float3 pos);
	void Rotate(float3 rot);
	void Scale(float3 scale);

	// Apply Transformations
	void UpdatePosition();
	void UpdateRotation();
	void UpdateScale();
	void UpdateTransform();

	// Matrix
	void SetTransformMatrix(float3 _position, float3 _rotation, float3 _scale);
	Transform GetGlobalTransform();

	// Parent
	void ParentPositionUpdate(float3 pos);
	void ParentRotationUpdate(float3 rot);
	void ParentScaleUpdate(float3 scale);
	void ParentTransformUpdate(float3 pos, float3 rot, float3 scale);

private:

	bool transformByQuat = true;
	float4x4 GO_matrix;
	Transform GO_trans;
	Transform GO_parentTrans;

	// MESH
public:

	void AddMesh(Mesh* m);

	void RenderMesh();
	void ShowMesh(bool visible) { renderMesh = visible; }

	Mesh* GetMesh() { return GO_mesh; }

private:

	Mesh* GO_mesh = nullptr;

	bool renderMesh;

	// TEXTURE
public:

	void AddTexture(uint texture);
	Texture* GetTexture() { return GO_texture; }

	void ShowTexture(bool visible) { renderTexture = visible; }

private:

	Texture* GO_texture = nullptr;
	Texture* GO_originalTexture = nullptr;

	bool renderTexture;

};