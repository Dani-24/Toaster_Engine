#pragma once

#include "Globals.h"
#include "ModuleMesh3D.h"
#include "ModuleTexture.h"

class GameObject
{
public:

	GameObject(std::string name, GameObject* parent);
	~GameObject();

	void SetPos(vec3 pos);
	void SetRot(vec3 rot);
	void SetScale(vec3 scale);
	vec3 GetPos() { return position; }
	vec3 GetRot() { return rotation; }
	vec3 GetScale() { return scale; }

	void AddMesh(Mesh* m);
	void AddTexture(uint texture);

	void DeleteThisGameObject();

	uint GetID() { return ID; }
	
	void SetParent(GameObject* parent);
	GameObject* GetParent() { return parent; }

	void AddChild(GameObject* child);
	void DeleteChild(GameObject* child);

	std::string GetName() { return name; }

private:
	uint ID;
	GameObject* parent;

public:

	std::string name;
	std::vector<GameObject*> childs;

public:
	vec3 position = vec3(0, 0, 0), 
		rotation = vec3(0, 0, 0), 
		scale = vec3(1, 1, 1);

	Mesh* GO_mesh = nullptr;
	uint GO_texture = NULL;

	bool pendindToDelete = false;
};