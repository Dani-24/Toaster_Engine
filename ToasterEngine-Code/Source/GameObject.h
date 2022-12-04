#pragma once

#include "Globals.h"

#include "ModuleMesh3D.h"
#include "ModuleTexture.h"
#include "ModuleCamera3D.h"

#include <math.h>
#include "../External/ImGui/imgui.h"

struct Transform {
	vec3 position = vec3(0.0f, 0.0f, 0.0f),
		rotation = vec3(0.0f, 0.0f, 0.0f),
		scale = vec3(1.0f, 1.0f, 1.0f);
};

class GameObject
{
public:
	GameObject(std::string name, GameObject* parent, Camera* cam = nullptr);
	~GameObject();

	void DeleteThisGameObject();

	// Hierarchy
private:
	void SetParent(GameObject* parent);

public:
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
	GameObject* parent = nullptr;

public:

	std::string name;
	std::vector<GameObject*> childs;

	bool pendindToDelete = false;

	// TRANSFORM
public:

	void SetPos(vec3 pos);
	void SetRot(vec3 rot);
	void SetScale(vec3 scale);

	vec3 GetPos() { return GO_trans.position; }
	vec3 GetRot() { return GO_trans.rotation; }
	vec3 GetScale() { return GO_trans.scale; }

	void SetTransform(vec3 pos, vec3 rot, vec3 scale);

	// Apply Transformations
	void UpdatePosition();
	void UpdateRotation();
	void UpdateScale();
	void UpdateTransform();

	// Matrix
	void SetTransformMatrix(vec3 _position, vec3 _rotation, vec3 _scale);
	void SetGlobalMatrix();
	Transform GetGlobalTransform();

	// Parent
	void ParentPositionUpdate(vec3 pos);
	void ParentRotationUpdate(vec3 rot);
	void ParentScaleUpdate(vec3 scale);
	void ParentTransformUpdate(vec3 pos, vec3 rot, vec3 scale);

public:
	mat4x4 GO_matrix;

private:
	Transform GO_trans;
	Transform GO_parentTrans, GO_parentOriginalTrans;

	Transform global_transform;

public:
	// MESH
	void AddMesh(Mesh* m);

	void RenderMesh();
	void ShowMesh(bool visible) { renderMesh = visible; }

	Mesh* GetMesh() { return GO_mesh; }

	void DisplayMesh(bool display);

private:

	Mesh* GO_mesh = nullptr;

	bool renderMesh;

public:
	// TEXTURE
	void AddTexture(Texture* texture);
	Texture* GetTexture() { return GO_texture; }
	void DeleteTextures();

private:

	std::vector<Texture*> GO_allTextures;
	Texture* GO_texture = nullptr;

	bool renderTexture;

	// Camera
public:
	Camera* GO_camera;

	float3 camLookAt = float3(0.0f, 1.0f, 0.0f);

	// AABB
public:
	void GenerateAABB();
	void DrawAABB();

	bool aabb_init = false;
	AABB aabb;
};