#include "Application.h"

#include "GameObject.h"
#include "ModuleEditor.h"

GameObject::GameObject(std::string name, GameObject* parent, Camera* camera)
{

	this->ID = app->editor->AddGameObject(this);

	this->name = name;

	if (parent != nullptr) {
		parent->AddChild(this);
		GO_parentTrans = parent->GO_trans;
	}

	GO_camera = camera;
	if (GO_camera != nullptr) {
		SetPos(GO_camera->GetPos());
	}

	GetGlobalTransform();

	CreateAABB();

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
	GO_texture = nullptr;
	GO_allTextures.clear();

	// Delete Cameras
	if (GO_camera != nullptr) {
		app->camera->activeCamera = nullptr;
		app->camera->DeleteCamera(GO_camera);
		GO_camera = nullptr;
	}

	// Delete childs / hierarchy
	if (!childs.empty()) {
		for (uint i = 0; i < childs.size(); i++)
		{
			childs[i]->pendindToDelete = true;
			//childs[i]->DeleteThisGameObject();
		}
	}
	if (parent != nullptr) {
		parent->DeleteChild(this);
	}
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

	if (chi->parent != nullptr) {
		chi->parent->DeleteChild(chi);
	}

	chi->SetParent(this);

	childs.push_back(chi);
}

void GameObject::SetParent(GameObject* par) {
	parent = par;
	GO_parentOriginalTrans = par->GetGlobalTransform();
}

// ImGUI
void GameObject::OnEditor() {

	// Transform Component
	ImGui::TextWrapped("Component : TRANSFORM"); 
	ImGui::NewLine();

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

	float3 scale = float3(GO_trans.scale.x, GO_trans.scale.y, GO_trans.scale.z);
	if (GO_camera == nullptr) {
		ImGui::TextWrapped("Scale :    ");
		ImGui::SameLine();
		
		if (ImGui::DragFloat3("scl", &scale[0], 0.1f)) {
			SetScale(vec3(scale.x, scale.y, scale.z));
		}
	}
	// MESH COMPONENT
	if (GO_mesh != nullptr) {

		app->editor->Space();

		ImGui::TextWrapped("Component : MESH"); ImGui::NewLine();
		ImGui::TextWrapped("Path : %s", GO_mesh->path.c_str());

		ImGui::TextWrapped("Show Mesh: ");
		ImGui::SameLine();

		ImGui::Selectable("Visible : ", &renderMesh);
		ImGui::SameLine();

		if (renderMesh) {
			ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.0f, 1.0f), "True");
		}
		else {
			ImGui::TextColored(ImVec4(1.f, 0.0f, 0.0f, 1.0f), "False");
		}

		DisplayMesh(renderMesh);

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

		if (ImGui::BeginCombo("Texture", GO_texture->name.c_str(), ImGuiComboFlags_HeightSmall))
		{
			for (int i = 0; i < GO_allTextures.size(); i++) {

				bool is_selected = (GO_texture == GO_allTextures[i]);
				if (ImGui::Selectable(GO_allTextures[i]->name.c_str(), is_selected))
				{
					GO_texture = GO_allTextures[i];
				}

			}
			ImGui::EndCombo();
		}

		ImGui::Image((ImTextureID)GO_texture->OpenGLID, ImVec2(100, 100));

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

	// CAMERA COMPONENT
	if (GO_camera != nullptr) {
		app->editor->Space();

		ImGui::TextWrapped("Component : Camera");
		ImGui::SameLine();
		if (ImGui::Checkbox("Active", &GO_camera->active)) {
			if (GO_camera->active) {
				app->camera->activeCamera = GO_camera;
				for (int i = 0; i < app->camera->cameras.size(); i++) {
					if (app->camera->cameras[i] != GO_camera) {
						app->camera->cameras[i]->active = false;
					}
				}
			}
		}

		ImGui::NewLine();

		ImGui::Image((ImTextureID)GO_camera->cameraBuffer.GetTexture(), ImVec2(200,100), ImVec2(0, 1), ImVec2(1, 0));
		
		ImGui::NewLine();

		ImGui::TextWrapped("Propierties :");

		ImGui::NewLine();

		ImGui::Checkbox("Frustum Culling", &GO_camera->frustumCulling);

		ImGui::NewLine();

		ImGui::TextWrapped("Aspect Ratio : ");
		ImGui::SameLine();
		if (ImGui::DragFloat("AR", &GO_camera->aspectRatio, 0.1f)) {
			GO_camera->SetAspectRatio(GO_camera->aspectRatio);
		}

		ImGui::TextWrapped("FOV :          ");
		ImGui::SameLine();
		float fov = math::RadToDeg(GO_camera->FOV);
		if (ImGui::DragFloat("FOV", &fov, 0.1f)) {
			GO_camera->SetFOV(fov);
		}

		ImGui::TextWrapped("Range :        ");
		ImGui::SameLine();
		if (ImGui::DragFloat("R", &GO_camera->range, 0.1f)) {
			GO_camera->SetRange(GO_camera->range);
		}

		ImGui::TextWrapped("Looking at :        ");
		if (ImGui::DragFloat3("XYZ", &camLookAt[0], 0.1f)) {
			GO_camera->LookAt(camLookAt);
		}
	}
}

// TRANSFORM
void GameObject::SetPos(vec3 pos) {
	if (app->editor->paused == false) {
		this->GO_trans.position = pos;

		UpdatePosition();
	}
}

void GameObject::SetRot(vec3 rot) {
	if (app->editor->paused == false) {
		this->GO_trans.rotation = rot;

		UpdateRotation();
	}
}

void GameObject::SetScale(vec3 scale) {
	if (app->editor->paused == false) {
		this->GO_trans.scale = scale;

		UpdateScale();
	}
}

void GameObject::SetTransform(vec3 pos, vec3 rot, vec3 scale) {
	this->GO_trans.position = pos;
	this->GO_trans.rotation = rot;
	this->GO_trans.scale = scale;
	UpdateTransform();
}

// Apply Transformations
void GameObject::UpdatePosition() {

	vec3 globalPosition = GO_parentTrans.position + GO_trans.position;

	for (size_t i = 0; i < childs.size(); i++)
	{
		childs[i]->ParentPositionUpdate(globalPosition);
	}

	// Camera
	if (GO_camera != nullptr) {
		GO_camera->camFrustum.pos = float3(globalPosition.x, globalPosition.y, globalPosition.z);
	}

	SetGlobalMatrix();
}

void GameObject::UpdateRotation() {

	vec3 globalRotation = GO_parentTrans.rotation + GO_trans.rotation;

	for (size_t i = 0; i < childs.size(); i++)
	{
		childs.at(i)->ParentRotationUpdate(globalRotation);
	}

	//Camera
	if (GO_camera != nullptr) {
		Quat dir;

		GO_camera->camFrustum.WorldMatrix().Decompose(float3(), dir, float3());

		Quat X = Quat::identity;
		X.SetFromAxisAngle(float3(1, 0, 0), globalRotation.x * DEGTORAD);

		dir = dir * X;

		Quat Y = Quat::identity;
		Y.SetFromAxisAngle(float3(0, 1, 0), globalRotation.y * DEGTORAD);

		dir = dir * Y;

		Quat Z = Quat::identity;
		Z.SetFromAxisAngle(float3(0, 0, 1), globalRotation.z * DEGTORAD);

		dir = dir * Z;

		float4x4 mat = GO_camera->camFrustum.WorldMatrix();
		mat.SetRotatePart(dir.Normalized());
		GO_camera->camFrustum.SetWorldMatrix(mat.Float3x4Part());
	}

	SetGlobalMatrix();
}

void GameObject::UpdateScale() {

	vec3 globalScale = GO_parentTrans.scale * GO_trans.scale;

	for (size_t i = 0; i < childs.size(); i++)
	{
		childs.at(i)->ParentScaleUpdate(globalScale);
	}

	SetGlobalMatrix();
}

void GameObject::UpdateTransform() {

	Transform globalTransform = GetGlobalTransform();

	for (size_t i = 0; i < childs.size(); i++)
	{
		childs.at(i)->ParentTransformUpdate(globalTransform.position, globalTransform.scale, globalTransform.rotation);
	}

	// Camera
	if (GO_camera != nullptr) {
		GO_camera->camFrustum.pos = float3(globalTransform.position.x, globalTransform.position.y, globalTransform.position.z);

		Quat dir;

		GO_camera->camFrustum.WorldMatrix().Decompose(float3(), dir, float3());

		Quat X = Quat::identity;
		X.SetFromAxisAngle(float3(1, 0, 0), globalTransform.rotation.x * DEGTORAD);

		dir = dir * X;

		Quat Y = Quat::identity;
		Y.SetFromAxisAngle(float3(0, 1, 0), globalTransform.rotation.y * DEGTORAD);

		dir = dir * Y;

		Quat Z = Quat::identity;
		Z.SetFromAxisAngle(float3(0, 0, 1), globalTransform.rotation.z * DEGTORAD);

		dir = dir * Z;

		float4x4 mat = GO_camera->camFrustum.WorldMatrix();
		mat.SetRotatePart(dir.Normalized());
		GO_camera->camFrustum.SetWorldMatrix(mat.Float3x4Part());
	}

	SetGlobalMatrix();
}

// Matrix
void GameObject::SetTransformMatrix(vec3 _position, vec3 _rotation, vec3 _scale)
{
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

	aabb.SetPos(float3(_position.x, _position.y, _position.z));
}

void GameObject::SetGlobalMatrix() {

	Transform gTrans = GetGlobalTransform();
	SetTransformMatrix(gTrans.position, gTrans.rotation, gTrans.scale);
}

Transform GameObject::GetGlobalTransform() {
	if (GetParent() == nullptr) return GO_trans;

	global_transform.position = GO_parentTrans.position - GO_parentOriginalTrans.position + GO_trans.position;
	global_transform.rotation = GO_parentTrans.rotation - GO_parentOriginalTrans.rotation + GO_trans.rotation;
	global_transform.scale = GO_parentTrans.scale * GO_trans.scale;

	return global_transform;
}

// SugarDaddy
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
	CreateAABB();
}

void GameObject::RenderMesh() {
	if (GO_mesh != nullptr && renderMesh)
	{
		if (renderTexture == true && GetTexture() != nullptr) {
			GO_mesh->Render(GetTexture()->OpenGLID, GO_matrix);
		}
		else {
			GO_mesh->Render(NULL, GO_matrix);
		}
	}
}

void GameObject::DisplayMesh(bool display) {
	if (GO_mesh != nullptr) {
		renderMesh = display;

		for (int i = 0; i < childs.size(); i++) {
			childs[i]->DisplayMesh(display);
		}
	}
}

// TEXTURE
void GameObject::AddTexture(Texture* t) {

	// Assign texture to childs when adding a texture to an empty object
	if (GO_mesh == nullptr && !childs.empty()) {
		for (int i = 0; i < childs.size(); i++) {
			childs[i]->AddTexture(t);
		}
	}

	if (GO_allTextures.empty()) {
		GO_texture = app->textures->CheckersImage();
		GO_texture->name = "Checkers";
		GO_allTextures.push_back(GO_texture);

		renderTexture = true;
	}
	GO_texture = t;
	GO_allTextures.push_back(GO_texture);
}

void GameObject::DeleteTextures() {
	GO_texture = nullptr;
	GO_allTextures.clear();
}

// Bounding Boxes
void GameObject::CreateAABB()
{
	if (GO_mesh != nullptr) {

		aabb.SetNegativeInfinity();
		aabb.Enclose((float3*)GO_mesh->vertices, GO_mesh->num_vertices);
	}
	else {
		aabb.SetNegativeInfinity();
		
		math::Polygon p;

		float3 pos = float3(GO_trans.position.x, GO_trans.position.y, GO_trans.position.z);

		p.p.push_back(float3(-1, -1, -1) + pos);
		p.p.push_back(float3(-1, -1, 1) + pos);
		p.p.push_back(float3(-1, 1, -1) + pos);
		p.p.push_back(float3(-1, 1, 1) + pos);
		p.p.push_back(float3(1, -1, -1) + pos);
		p.p.push_back(float3(1, -1, 1) + pos);
		p.p.push_back(float3(1, 1, -1) + pos);
		p.p.push_back(float3(1, 1, 1) + pos);

		aabb.Enclose(p);
	}
}

void GameObject::DrawAABB() {
	if (this != app->editor->root) {
			float3 corners[8];
			float3 frustum_corners[8];

			// Get Frustum corners
			corners[0] = aabb.CornerPoint(0);
			corners[1] = aabb.CornerPoint(2);
			corners[2] = aabb.CornerPoint(4);
			corners[3] = aabb.CornerPoint(6);
			corners[4] = aabb.CornerPoint(1);
			corners[5] = aabb.CornerPoint(3);
			corners[6] = aabb.CornerPoint(5);
			corners[7] = aabb.CornerPoint(7);

			std::vector<float3> frustum_lines;

			frustum_lines.push_back(corners[0]);
			frustum_lines.push_back(corners[1]);
			frustum_lines.push_back(corners[0]);
			frustum_lines.push_back(corners[2]);
			frustum_lines.push_back(corners[1]);
			frustum_lines.push_back(corners[3]);
			frustum_lines.push_back(corners[2]);
			frustum_lines.push_back(corners[3]);

			frustum_lines.push_back(corners[4]);
			frustum_lines.push_back(corners[5]);
			frustum_lines.push_back(corners[4]);
			frustum_lines.push_back(corners[6]);
			frustum_lines.push_back(corners[5]);
			frustum_lines.push_back(corners[7]);
			frustum_lines.push_back(corners[6]);
			frustum_lines.push_back(corners[7]);

			frustum_lines.push_back(corners[0]);
			frustum_lines.push_back(corners[4]);
			frustum_lines.push_back(corners[1]);
			frustum_lines.push_back(corners[5]);
			frustum_lines.push_back(corners[2]);
			frustum_lines.push_back(corners[6]);
			frustum_lines.push_back(corners[3]);
			frustum_lines.push_back(corners[7]);

			// Add Lines to the DrawLines queue
			for (int i = 0; i < frustum_lines.size(); i++) {
				if (GO_camera == nullptr && GO_mesh != nullptr) {
					app->scene->AddLines(frustum_lines[i], Red);
				}
				else if (GO_camera == nullptr) {
					app->scene->AddLines(frustum_lines[i], Orange);
				}
				else {
					app->scene->AddLines(frustum_lines[i], Green);
				}
			}
	}
}