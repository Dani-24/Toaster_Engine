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
	for (size_t i = 0; i < childs.size(); i++)
	{
		RELEASE(childs[i]);
	}
	childs.clear();
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

// ImGUI
void GameObject::OnEditor() {
	// Transform Component
	ImGui::TextWrapped("Component : TRANSFORM"); ImGui::NewLine();

	ImGui::TextWrapped("Position : ");
	ImGui::SameLine();

	float3 pos = float3(GO_trans.position.x, GO_trans.position.y, GO_trans.position.z);
	if (ImGui::DragFloat3("pos", &pos[0], 0.1f)) {
		SetPos(pos);
	}

	ImGui::TextWrapped("Rotation : ");
	ImGui::SameLine();
	float3 rot = float3(GO_trans.rotation.x, GO_trans.rotation.y, GO_trans.rotation.z);
	if (ImGui::DragFloat3("rot", &rot[0], 0.1f)) {
		SetRot(rot);
	}

	ImGui::TextWrapped("Scale :    ");
	ImGui::SameLine();
	float3 scale = float3(GO_trans.scale.x, GO_trans.scale.y, GO_trans.scale.z);
	if (ImGui::DragFloat3("scl", &scale[0], 0.1f)) {
		SetScale(scale);
	}

	SetTransformMatrix(pos, rot, scale);

	// MESH COMPONENT
	if (GO_mesh != nullptr) {

		ImGui::TextWrapped("Component : MESH"); ImGui::NewLine();
		ImGui::TextWrapped("Path : %s", GO_mesh->path.c_str());

		ImGui::TextWrapped("Show Mesh: ");
		ImGui::SameLine();

		ImGui::Selectable("Visible : ", &GO_mesh->shouldRender);
		ImGui::SameLine();
		if (GO_mesh->shouldRender) {
			ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.0f, 1.0f), "True");
		}
		else {
			ImGui::TextColored(ImVec4(1.f, 0.0f, 0.0f, 1.0f), "False");
		}

		// Delete Mesh

		bool deleteMesh = false;
		ImGui::Selectable("Delete Component", &deleteMesh);

		if (deleteMesh) {
			GO_mesh = nullptr;
		}
	}

	// TEXTURE COMPONENT
	if (GO_texture != nullptr) {
		ImGui::TextWrapped("Component : TEXTURES");

		if (ImGui::BeginCombo("Texture", "Select", ImGuiComboFlags_HeightSmall))
		{
			/*bool is_selected = (GO_texture->OpenGLID == originalTexture);
			if (ImGui::Selectable("Default", is_selected))
			{
				GO_texture->OpenGLID = originalTexture;
			}
			is_selected = (GO_texture->OpenGLID == app->textures->checkers_texture);
			if (ImGui::Selectable("Checkers", is_selected))
			{
				GO_texture->OpenGLID = app->textures->checkers_texture;
			}*/
			ImGui::EndCombo();
		}
		ImGui::TextWrapped("Show Texture: ");
		ImGui::SameLine();
		ImGui::Selectable("Visible : ", &renderTexture);
		ImGui::SameLine();
		if (renderTexture) {
			ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.0f, 1.0f), "True");
		}
		else {
			ImGui::TextColored(ImVec4(1.f, 0.0f, 0.0f, 1.0f), "False");
		}

		// Delete Texture

		bool deleteTexture = false;
		ImGui::Selectable("Delete Component ", &deleteTexture);

		if (deleteTexture) {
			GO_texture = nullptr;
		}
	}
}

// TRANSFORM

void GameObject::SetPos(float3 pos) {
	this->GO_trans.position = pos;
	UpdatePosition();
}

void GameObject::SetRot(float3 rot) {
	this->GO_trans.rotation = rot;
	UpdateRotation();
}

void GameObject::SetScale(float3 scale) {
	this->GO_trans.scale = scale;
	UpdateScale();
}

void GameObject::SetTransform(float3 pos, float3 rot, float3 scale) {
	this->GO_trans.position = pos;
	this->GO_trans.rotation = rot;
	this->GO_trans.scale = scale;
	UpdateTransform();
}

void GameObject::Translate(float3 pos) {
	this->GO_trans.position += pos;
	UpdatePosition();
}

void GameObject::Rotate(float3 rot) {
	this->GO_trans.rotation += rot;
	UpdateRotation();
}

void GameObject::Scale(float3 scale) {
	this->GO_trans.scale += scale;
	UpdateScale();
}

// Apply Transformations
void GameObject::UpdatePosition() {
	float3 globalPosition = GO_parentTrans.position + GO_trans.position;

	for (size_t i = 0; i < childs.size(); i++)
	{
		childs.at(i)->ParentPositionUpdate(globalPosition);
	}
}

void GameObject::UpdateRotation() {
	float3 globalRotation = GO_parentTrans.rotation + GO_trans.rotation;

	for (size_t i = 0; i < childs.size(); i++)
	{
		childs.at(i)->ParentRotationUpdate(globalRotation);
	}
}

void GameObject::UpdateScale() {
	float3 globalScale = GO_parentTrans.scale + GO_trans.scale;

	for (size_t i = 0; i < childs.size(); i++)
	{
		childs.at(i)->ParentScaleUpdate(globalScale);
	}
}

void GameObject::UpdateTransform() {
	Transform globalTransform = GetGlobalTransform();

	for (size_t i = 0; i < childs.size(); i++)
	{
		childs.at(i)->ParentTransformUpdate(globalTransform.position, globalTransform.scale, globalTransform.rotation);
	}
}

// Matrix
void GameObject::SetTransformMatrix(float3 _position, float3 _rotation, float3 _scale)
{
	if (transformByQuat) {
		Transform globalTransform = GetGlobalTransform();

		math::Quat rotation = Quat::FromEulerXYZ(math::DegToRad(globalTransform.rotation.x), math::DegToRad(globalTransform.rotation.y), math::DegToRad(globalTransform.rotation.z));

		GO_matrix = float4x4::FromTRS(globalTransform.position, rotation, float3(1, 1, 1));
	}
	else {
		float x = _rotation.x * DEGTORAD;
		float y = _rotation.y * DEGTORAD;
		float z = _rotation.z * DEGTORAD;

		GO_matrix.At(0, 0) = cos(y) * cos(z);
		GO_matrix.At(0, 1) = -cos(x) * sin(z) + sin(y) * cos(z) * sin(x);
		GO_matrix.At(0, 2) = sin(x) * sin(z) + sin(y) * cos(z) * cos(x);
		GO_matrix.At(0, 3) = _position.x;

		GO_matrix.At(1, 0) = cos(y) * sin(z);
		GO_matrix.At(1, 1) = cos(x) * cos(z) + sin(y) * sin(z) * sin(z);
		GO_matrix.At(1, 2) = -sin(x) * cos(z) + sin(y) * sin(z) * cos(x);
		GO_matrix.At(1, 3) = _position.y;

		GO_matrix.At(2, 0) = -sin(y);
		GO_matrix.At(2, 1) = cos(y) * sin(x);
		GO_matrix.At(2, 2) = cos(x) * cos(y);
		GO_matrix.At(2, 3) = _position.z;

		GO_matrix.At(3, 0) = 0;
		GO_matrix.At(3, 1) = 0;
		GO_matrix.At(3, 2) = 0;
		GO_matrix.At(3, 3) = 1;

		GO_matrix.At(0, 0) *= _scale.x;
		GO_matrix.At(1, 1) *= _scale.y;
		GO_matrix.At(2, 2) *= _scale.z;

		GO_matrix.Transpose();
	}
}
	
Transform GameObject::GetGlobalTransform() {
	if (GetParent() == NULL) return GO_trans;

	Transform global_transform;
	global_transform.position = GO_parentTrans.position + GO_trans.position;
	global_transform.rotation = GO_parentTrans.rotation + GO_trans.rotation;
	global_transform.scale = GO_parentTrans.scale + GO_trans.scale;

	return global_transform;
}

// Padre
void GameObject::ParentPositionUpdate(float3 pos) {
	GO_trans.position = pos;
	UpdatePosition();
}

void GameObject::ParentRotationUpdate(float3 rot) {
	GO_trans.rotation = rot;
	UpdateRotation();
}

void GameObject::ParentScaleUpdate(float3 scale) {
	GO_trans.scale = scale;
	UpdateScale();
}

void GameObject::ParentTransformUpdate(float3 pos, float3 rot, float3 scale) {
	GO_parentTrans.position = pos;
	GO_parentTrans.rotation = rot;
	GO_parentTrans.scale = scale;

	UpdateTransform();
}

// MESH

void GameObject::AddMesh(Mesh* m) {
	GO_mesh = m;
}

void GameObject::RenderMesh() {
	if (GO_mesh->shouldRender) 
	{
		if (renderTexture) {
			GO_mesh->Render(GetTexture()->OpenGLID);
		}
		else {
			GO_mesh->Render(NULL);
		}
	}
}

// TEXTURE

void GameObject::AddTexture(uint t) {
	GO_texture->OpenGLID = t;
}