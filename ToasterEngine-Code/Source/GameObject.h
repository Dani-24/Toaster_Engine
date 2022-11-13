#pragma once

#include "Globals.h"
#include "ModuleMesh3D.h"
#include "ModuleTexture.h"

#include "Component.h"
#include "Comp_Mesh.h"
#include "Comp_Texture.h"
#include "Comp_Transform.h"

class GameObject
{
public:

	GameObject(std::string name, GameObject* parent);
	~GameObject();

	void DeleteThisGameObject();

	uint GetID() { return ID; }
	
	void SetParent(GameObject* parent);
	GameObject* GetParent() { return parent; }

	void AddChild(GameObject* child);
	void DeleteChild(GameObject* child);

	std::string GetName() { return name; }

	Component* AddComponent(Component::Comp_Type ctype, Mesh* m = nullptr, uint t = NULL)
	{
		Component* new_component;
		switch (ctype)
		{
		case Component::Comp_Type::Transform:
			new_component = new Comp_Transform(this);
			break;
		case Component::Comp_Type::Mesh:
			new_component = new Comp_Mesh(this, m);
			break;
		case Component::Comp_Type::Texture:
			new_component = new Comp_Texture(this, t);
			break;
		default:
			LOG("Error adding a new component");
			break;
		}
		components.push_back(new_component);
		return new_component;
	}
	Component* GetComponent(Component::Comp_Type ctype);

	void RemoveComponent(Component* component)
	{
		int pos_in_array = 0;
		for (size_t i = 0; i < components.size(); i++)
		{
			if (components.at(i) == component)
			{
				components.erase(components.begin() + pos_in_array);
				return;
			}
			else
			{
				pos_in_array++;
			}
		}
	}

private:
	uint ID;
	GameObject* parent;

public:
	std::string name;
	std::vector<GameObject*> childs;
	std::vector<Component*> components;

public:

	bool pendindToDelete = false;

	bool renderText;
};