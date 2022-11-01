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

void GameObject::SetTransformMatrix(vec3 _position, vec3 _rotation, vec3 _scale)
{
	float x = _rotation.x * DEGTORAD;
	float y = _rotation.y * DEGTORAD;
	float z = _rotation.z * DEGTORAD;

	lTransform[0] = cos(y) * cos(z);
	lTransform[1] = -cos(x) * sin(z) + sin(y) * cos(z) * sin(x);
	lTransform[2] = sin(x) * sin(z) + sin(y) * cos(z) * cos(x);
	lTransform[3] = _position.x;

	lTransform[4] = cos(y) * sin(z);
	lTransform[5] = cos(x) * cos(z) + sin(y) * sin(z) * sin(z);
	lTransform[6] = -sin(x) * cos(z) + sin(y) * sin(z) * cos(x);
	lTransform[7] = _position.y;

	lTransform[8] = -sin(y);
	lTransform[9] = cos(y) * sin(x);
	lTransform[10] = cos(x) * cos(y);
	lTransform[11] = _position.z;

	lTransform[12] = 0;
	lTransform[13] = 0;
	lTransform[14] = 0;
	lTransform[15] = 1;

	lTransform[0] *= _scale.x;
	lTransform[5] *= _scale.y;
	lTransform[10] *= _scale.z;

	lTransform = transpose(lTransform);
}