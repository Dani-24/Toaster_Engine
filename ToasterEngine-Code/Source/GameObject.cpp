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

	LOG("Created GameObject %s", name.c_str());

	app->editor->SetSelectedGameObject(this);
}

GameObject::~GameObject()
{
}

void GameObject::SetPos(vec3 pos) {
	position = pos;
}
void GameObject::SetRot(vec3 rot) {
	rotation = rot;
}
void GameObject::SetScale(vec3 scale) {
	this->scale = scale;
}

void GameObject::AddMesh(Mesh* m) {
	GO_mesh = m;
}

void GameObject::AddTexture(uint t) {
	GO_texture = t;
}

void GameObject::DeleteThisGameObject() {
	app->editor->SetSelectedGameObject(nullptr);
	parent->DeleteChild(this);
	for (size_t i = 0; i < childs.size(); i++)
	{
		childs[i]->DeleteThisGameObject();
	}

	// This works?
	ID = NULL;
	delete this;
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