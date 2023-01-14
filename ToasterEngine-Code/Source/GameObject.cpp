#include "GameObject.h"
#include "Component.h"

#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Texture.h"
#include "C_Camera.h"
#include "C_Animator.h"

//#include "MO_Scene.h"

//#include "IM_PrefabImporter.h"

#include <algorithm>

#include "Application.h"

GameObject::GameObject(const char* _name, GameObject* parent, int _uid) : parent(parent), name(_name), showChildren(false),
active(true), toDelete(false), dontDestroy(false), UID(_uid), transform(nullptr), dumpComponent(nullptr)
{
	if (parent != nullptr)
		parent->children.push_back(this);

	transform = dynamic_cast<C_Transform*>(AddComponent(Component::TYPE::TRANSFORM));

	if (UID == -1)
	{
		UID = app->editor->SomeRandomIntValue();
	}
}

GameObject::~GameObject()
{
	if (app->editor->GetSelectedGameObject() == this)
		app->editor->SetSelectedGameObject(nullptr);

	for (size_t i = 0; i < components.size(); i++)
	{
		delete components[i];
		components[i] = nullptr;
	}
	components.clear();

	for (size_t i = 0; i < children.size(); i++)
	{
		delete children[i];
		children[i] = nullptr;
	}
	children.clear();
}

void GameObject::Update()
{
	if (dumpComponent != nullptr)
	{
		components.erase(std::find(components.begin(), components.end(), dumpComponent));
		delete dumpComponent;
		dumpComponent = nullptr;
	}

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->IsActive())
			components[i]->Update();
	}
}

void GameObject::PostUpdate()
{
	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->IsActive())
			components[i]->PostUpdate();
	}
}

Component* GameObject::AddComponent(Component::TYPE _type, const char* params)
{
	assert(_type != Component::TYPE::NONE, "Can't create a NONE component");
	Component* ret = nullptr;

	switch (_type)
	{
	case Component::TYPE::TRANSFORM:
		if (transform == nullptr)
			ret = new C_Transform(this);
		break;
	case Component::TYPE::MESH:
		ret = new C_Mesh(this);
		break;
	case Component::TYPE::MATERIAL:
		ret = new C_Material(this);
		break;
	case Component::TYPE::CAMERA:
		ret = new C_Camera(this);
		//EngineExternal->moduleScene->SetGameCamera(dynamic_cast<C_Camera*>(ret));
		break;
	case Component::TYPE::ANIMATOR:
		ret = new C_Animator(this);
		break;
	}

	if (ret != nullptr)
	{
		ret->type = _type;
		components.push_back(ret);
	}

	return ret;
}

Component* GameObject::GetComponent(Component::TYPE _type)
{
	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i] && components[i]->type == _type)
		{
			return components[i];
		}
	}

	return nullptr;
}

std::vector<Component*> GameObject::GetComponentsOfType(Component::TYPE type)
{
	std::vector< Component*> ret;
	for (size_t i = 0; i < components.size(); ++i)
	{
		if (components[i]->type == type)
			ret.push_back(components[i]);
	}
	return ret;
}


//When we load models from model trees the UID should get regenerated
//because the .model UID are not unique.
void GameObject::RecursiveUIDRegeneration()
{
	this->UID = app->editor->SomeRandomIntValue();

	for (size_t i = 0; i < this->children.size(); i++)
	{
		this->children[i]->RecursiveUIDRegeneration();
	}
}


void GameObject::RecursiveUIDRegenerationSavingReferences(std::map<uint, GameObject*>& gameObjects)
{
	gameObjects[UID] = this;
	UID = app->editor->SomeRandomIntValue();

	for (size_t i = 0; i < this->children.size(); i++)
	{
		this->children[i]->RecursiveUIDRegenerationSavingReferences(gameObjects);
	}
}

bool GameObject::isActive() const
{
	return active;
}

void GameObject::Enable()
{
	active = true;

	if (parent != nullptr)
		parent->Enable();
}

void GameObject::Disable()
{
	active = false;
}

bool GameObject::IsRoot()
{
	return (parent == nullptr) ? true : false;
}

void GameObject::Destroy()
{
	toDelete = true;
}

void GameObject::GetChildrenUIDs(std::vector<uint>& UIDs)
{
	for (size_t i = 0; i < children.size(); i++)
	{
		UIDs.push_back(children[i]->UID);
		children[i]->GetChildrenUIDs(UIDs);
	}
}

void GameObject::RemoveComponent(Component* ptr)
{
	dumpComponent = ptr;
}

//TODO: WTF IS GOING ON WITH THE ARNAU BUG FFS -> Un grande Arnau
//Deparenting objects with deformations grows transforms
void GameObject::ChangeParent(GameObject* newParent)
{
	//GameObject* ret = nullptr;
	//ret = IsChild(newParent, ret);
	if (IsChild(newParent))
		return;

	if (parent != nullptr)
		parent->RemoveChild(this);

	parent = newParent;
	parent->children.push_back(this);

	//TODO: This could be improved, you are setting up the local matrix 2 times
	transform->localTransform = parent->transform->globalTransform.Inverted() * transform->globalTransform;

	Quat _rot;
	float3 scale, pos;
	transform->localTransform.RotatePart().Decompose(_rot, scale);

	transform->SetTransformMatrix(transform->localTransform.TranslatePart(), _rot, scale);
	transform->updateTransform = true;
}

bool GameObject::IsChild(GameObject* _toFind)
{
	if (_toFind == nullptr)
		return false;

	if (_toFind == this)
	{
		return true;
	}
	else
	{
		return IsChild(_toFind->parent);
	}
}


void GameObject::RemoveChild(GameObject* child)
{
	child->parent = nullptr;
	children.erase(std::find(children.begin(), children.end(), child));
}

void GameObject::CollectChilds(std::vector<GameObject*>& vector)
{
	vector.push_back(this);
	for (uint i = 0; i < children.size(); i++)
		children[i]->CollectChilds(vector);
}

GameObject* GameObject::GetChild(std::string& childName)
{
	for (size_t i = 0; i < children.size(); i++)
	{
		if (children[i]->name == childName)
			return children[i];

		GameObject* child = children[i]->GetChild(childName);

		if (child != nullptr)
			return child;
	}

	return nullptr;
}

void GameObject::OnEditor() 
{
	ImGui::InputText(" ", (char*)name.c_str(), 50);

	ImGui::SameLine();

	ImGui::TextWrapped("ID: %d", UID);

	for (int i = 0; i < components.size(); i++) {
		components[i]->OnEditor();
	}
}

void GameObject::DrawAABB() {
	for (int i = 0; i < components.size(); i++) {
		if (components[i]->type == Component::TYPE::MESH) {
			components[i]->DrawAABB();
		}
	}
}

AABB GameObject::GetAABB() {
	for (int i = 0; i < components.size(); i++) {
		if (components[i]->type == Component::TYPE::MESH) {
			components[i]->GetAABB();
		}
	}
}

Camera* GameObject::GOCamera() {
	for (int i = 0; i < components.size(); i++) {
		if (components[i]->type == Component::TYPE::CAMERA) {
			components[i]->GetCamera();
		}
	}
}