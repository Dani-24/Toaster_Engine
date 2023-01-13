#pragma once

#include "GameObject.h"
#include "Globals.h"

class GameObject;

class Component
{
public:

	enum class TYPE
	{
		NONE,
		TRANSFORM,
		MESH_RENDERER,
		MATERIAL,
		CAMERA,
		ANIMATOR,
	};

	Component(GameObject* _gm);
	virtual ~Component();

	virtual void Enable();
	virtual void Disable();

	virtual void Update();
	virtual void PostUpdate();

	virtual void OnRecursiveUIDChange(std::map < uint, GameObject*> gameObjects);

	virtual bool OnEditor();

	inline bool IsActive() {
		return active;
	}

	inline GameObject* GetGO() {
		return gameObject;
	}

	inline std::string GetName() {
		return name;
	}

	TYPE type;

protected:

	bool active;
	GameObject* gameObject;
	std::string name;
};