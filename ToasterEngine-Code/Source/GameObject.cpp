#include "Application.h"

#include "GameObject.h"
#include "ModuleEditor.h"

GameObject::GameObject(std::string name, GameObject* parent)
{
	
	this->ID = app->editor->AddGameObject(this);

	this->name = name;

	if (parent != nullptr) {
		parent->AddChild(this);
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

	// Deselect if selected
	if (app->editor->selectedGameObj == this) {
		app->editor->SetSelectedGameObject(nullptr);
	}

	// Clean meshes / textures
	if (GO_mesh != nullptr) {
		GO_mesh->~Mesh();
		GO_mesh = nullptr;
	}
	GO_texture = NULL;

	// Delete from hierarchy
	for (size_t i = 0; i < childs.size(); i++)
	{
		childs[i]->DeleteThisGameObject();
	}
	parent->DeleteChild(this);
}

void GameObject::DeleteChild(GameObject* chi) {

	for (size_t i = 0; i < childs.size(); i++)
	{
		if (chi == childs[i]) { 
			childs.erase(childs.begin() + i); 
		}
	}
	chi->parent = nullptr;
}

void GameObject::AddChild(GameObject* chi) {
	chi->SetParent(this);
	childs.push_back(chi);
}

void GameObject::SetParent(GameObject* par) {
	parent = par;
}

// ImGUI
void GameObject::OnEditor() {
	// Transform Component
	ImGui::TextWrapped("Component : TRANSFORM"); ImGui::NewLine();

	ImGui::TextWrapped("Position : ");
	ImGui::SameLine();

	float3 pos = float3(GO_trans.position.x, GO_trans.position.y, GO_trans.position.z);
	if (ImGui::DragFloat3("pos", &pos[0], 0.1f)) {
		SetPos(vec3(pos.x, pos.y, pos.z));
	}

	ImGui::TextWrapped("Rotation : ");
	ImGui::SameLine();
	float3 rot = float3(GO_trans.rotation.x, GO_trans.rotation.y, GO_trans.rotation.z);
	if (ImGui::DragFloat3("rot", &rot[0], 0.1f)) {
		SetRot(vec3(rot.x, rot.y, rot.z));
	}

	ImGui::TextWrapped("Scale :    ");
	ImGui::SameLine();
	float3 scale = float3(GO_trans.scale.x, GO_trans.scale.y, GO_trans.scale.z);
	if (ImGui::DragFloat3("scl", &scale[0], 0.1f)) {
		SetScale(vec3(scale.x, scale.y, scale.z));
	}

	// MESH COMPONENT
	if (GO_mesh != nullptr) {

		app->editor->Space();

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

		DisplayMesh(GO_mesh->shouldRender);

		// Delete Mesh

		bool deleteMesh = false;
		ImGui::Selectable("Delete Component", &deleteMesh);

		if (deleteMesh) {
			GO_mesh = nullptr;
		}
	}

	// TEXTURE COMPONENT
	if (GO_texture != nullptr) {

		app->editor->Space();

		ImGui::TextWrapped("Component : TEXTURES");

		if (ImGui::BeginCombo("Texture", "Select", ImGuiComboFlags_HeightSmall))
		{
			bool is_selected = (GO_texture->OpenGLID == GO_originalTexture->OpenGLID);
			if (ImGui::Selectable("Default", is_selected))
			{
				GO_texture->OpenGLID = GO_originalTexture->OpenGLID;
			}
			is_selected = (GO_texture->OpenGLID == app->textures->CheckImage());
			if (ImGui::Selectable("Checkers", is_selected))
			{
				GO_texture->OpenGLID = app->textures->CheckImage();
			}
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
void GameObject::SetPos(vec3 pos) {
	this->GO_trans.position = pos;

	UpdatePosition();
}

void GameObject::SetRot(vec3 rot) {
	this->GO_trans.rotation = rot;

	UpdateRotation();
}

void GameObject::SetScale(vec3 scale) {
	this->GO_trans.scale = scale;

	UpdateScale();
}

void GameObject::SetTransform(vec3 pos, vec3 rot, vec3 scale) {
	this->GO_trans.position = pos;
	this->GO_trans.rotation = rot;
	this->GO_trans.scale = scale;
	UpdateTransform();
}

void GameObject::Translate(vec3 pos) {
	this->GO_trans.position += pos;

	UpdatePosition();
}

void GameObject::Rotate(vec3 rot) {
	this->GO_trans.rotation += rot;

	UpdateRotation();
}

void GameObject::Scale(vec3 scale) {
	this->GO_trans.scale += scale;

	UpdateScale();
}

// Apply Transformations
void GameObject::UpdatePosition() {

	UpdateMatrix();

	vec3 globalPosition = GO_parentTrans.position + GO_trans.position;

	for (size_t i = 0; i < childs.size(); i++)
	{
		childs.at(i)->ParentPositionUpdate(globalPosition);
	}
}

void GameObject::UpdateRotation() {

	UpdateMatrix();

	vec3 globalRotation = GO_parentTrans.rotation + GO_trans.rotation;

	for (size_t i = 0; i < childs.size(); i++)
	{
		childs.at(i)->ParentRotationUpdate(globalRotation);
	}
}

void GameObject::UpdateScale() {

	UpdateMatrix();

	vec3 globalScale = GO_parentTrans.scale + GO_trans.scale;

	for (size_t i = 0; i < childs.size(); i++)
	{
		childs.at(i)->ParentScaleUpdate(globalScale);
	}
}

void GameObject::UpdateTransform() {

	UpdateMatrix();

	Transform globalTransform = GetGlobalTransform();

	for (size_t i = 0; i < childs.size(); i++)
	{
		childs.at(i)->ParentTransformUpdate(globalTransform.position, globalTransform.scale, globalTransform.rotation);
	}
}

// Matrix
void GameObject::UpdateMatrix() {
	SetTransformMatrix(GO_trans.position, GO_trans.rotation, GO_trans.scale);
}

void GameObject::SetTransformMatrix(vec3 _position, vec3 _rotation, vec3 _scale)
{
	if (transformByQuat) {
		/*Transform globalTransform = GetGlobalTransform();

		math::Quat rotation = Quat::FromEulerXYZ(math::DegToRad(globalTransform.rotation.x), math::DegToRad(globalTransform.rotation.y), math::DegToRad(globalTransform.rotation.z));

		GO_matrix = float4x4::FromTRS(globalTransform.position, rotation, float3(1, 1, 1));*/
	}
	else {
		float x = _rotation.x * DEGTORAD;
		float y = _rotation.y * DEGTORAD;
		float z = _rotation.z * DEGTORAD;

		GO_matrix[0] = cos(y) * cos(z);
		GO_matrix[1] = -cos(x) * sin(z) + sin(y) * cos(z) * sin(x);
		GO_matrix[2] = sin(x) * sin(z) + sin(y) * cos(z) * cos(x);
		GO_matrix[3] = _position.x;

		GO_matrix[4] = cos(y) * sin(z);
		GO_matrix[5] = cos(x) * cos(z) + sin(y) * sin(z) * sin(z);
		GO_matrix[6] = -sin(x) * cos(z) + sin(y) * sin(z) * cos(x);
		GO_matrix[7] = _position.y;

		GO_matrix[8] = -sin(y);
		GO_matrix[9] = cos(y) * sin(x);
		GO_matrix[10] = cos(x) * cos(y);
		GO_matrix[11] = _position.z;

		GO_matrix[12] = 0;
		GO_matrix[13] = 0;
		GO_matrix[14] = 0;
		GO_matrix[15] = 1;

		GO_matrix[0] *= _scale.x;
		GO_matrix[5] *= _scale.y;
		GO_matrix[10] *= _scale.z;

		GO_matrix = transpose(GO_matrix);
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

// Padre te ordena
void GameObject::ParentPositionUpdate(vec3 pos) {
	GO_parentTrans.position = pos;

	UpdatePosition();
}

void GameObject::ParentRotationUpdate(vec3 rot) {
	GO_parentTrans.rotation = rot;

	UpdateRotation();
}

void GameObject::ParentScaleUpdate(vec3 scale) {
	GO_parentTrans.scale = scale;

	UpdateScale();
}

void GameObject::ParentTransformUpdate(vec3 pos, vec3 rot, vec3 scale) {
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
	if (GO_mesh != nullptr && GO_mesh->shouldRender) 
	{
		if (renderTexture && GetTexture() != nullptr) {
			GO_mesh->Render(GetTexture()->OpenGLID, GO_matrix);
		}
		else {
			GO_mesh->Render(NULL, GO_matrix);
		}
	}
}

void GameObject::DisplayMesh(bool display) {

	GO_mesh->shouldRender = display;

	for (int i = 0; i < childs.size(); i++) {
		childs[i]->DisplayMesh(display);
	}
}

// TEXTURE
void GameObject::AddTexture(uint t) {
	GO_texture = new Texture();
	GO_texture->OpenGLID = t;
}