#include "Application.h"

#include "GameObject.h"
#include "ModuleEditor.h"

GameObject::GameObject(std::string name, GameObject* parent)
{
	
	this->ID = app->editor->AddGameObject(this);

	this->name = name;

	if (parent != nullptr) {
		parent->AddChild(this);
		this->SetParent(parent);
	}

	AddComponent(Component::Comp_Type::Transform);

	LOG("Created GameObject %s", name.c_str());

	app->editor->SetSelectedGameObject(this);
}

GameObject::~GameObject()
{
	for (size_t i = 0; i < childs.size(); i++)
	{
		RELEASE(childs[i]);
	}
	childs.clear();

	for (size_t i = 0; i < components.size(); i++)
	{
		RELEASE(components[i]);
	}
	components.clear();
}

void GameObject::DeleteThisGameObject() {

	LOG("Deleting Game Object %s", name.c_str());

	app->editor->SetSelectedGameObject(nullptr);
	parent->DeleteChild(this);
	for (size_t i = 0; i < childs.size(); i++)
	{
		childs[i]->DeleteThisGameObject();
	}
}

void GameObject::SetParent(GameObject* par) {
	parent = par;
	parent->childs.push_back(this);
}

void GameObject::AddChild(GameObject* chi) {
	chi->parent = this;
	childs.push_back(chi);
}

void GameObject::DeleteChild(GameObject* chi) {

	for (size_t i = 0; i < childs.size(); i++)
	{
		if (chi->ID == childs[i]->ID) childs.erase(childs.begin() + i);
	}
	chi->parent = nullptr;
}