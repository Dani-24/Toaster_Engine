#pragma once

#include "Globals.h"
#include "ModuleMesh3D.h"
#include "ModuleTexture.h"

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

	void Translate(vec3 pos);
	void Rotate(vec3 rot);
	void Scale(vec3 scale);

	// Apply Transformations
	void UpdatePosition();
	void UpdateRotation();
	void UpdateScale();
	void UpdateTransform();

	// Matrix
	void SetTransformMatrix(vec3 _position, vec3 _rotation, vec3 _scale);
	Transform GetGlobalTransform();

	// Parent
	void ParentPositionUpdate(vec3 pos);
	void ParentRotationUpdate(vec3 rot);
	void ParentScaleUpdate(vec3 scale);
	void ParentTransformUpdate(vec3 pos, vec3 rot, vec3 scale);

private:

	bool transformByQuat = false;
	mat4x4 GO_matrix;
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
	Texture* GO_originalTexture = new Texture();

	bool renderTexture;

};