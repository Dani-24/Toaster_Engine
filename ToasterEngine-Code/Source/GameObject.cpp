#include "Application.h"

#include "GameObject.h"
#include "ModuleEditor.h"

GameObject::GameObject(std::string name, GameObject* parent, Camera* camera)
{

	this->ID = app->editor->AddGameObject(this);

	this->name = name;

	if (parent != nullptr) {
		parent->AddChild(this);
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

	// Delete animations
	if (!GO_animations.empty()) {
		GO_animations.clear();
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

	ImGui::NewLine();

	if (ImGui::Button("Reset Transform", ImVec2(ImGui::GetWindowSize().x, 20))) {
		SetPos(vec3(0, 0, 0));
		SetRot(vec3(0, 0, 0));
		SetScale(vec3(1, 1, 1));
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

	// ANIMATION COMPONENT
	if (!GO_animations.empty()) {
		app->editor->Space();

		ImGui::TextWrapped("Component : ANIMATION"); ImGui::NewLine();
		
		ImGui::Checkbox("Show Bones", &drawBones);
		
		if (rootBone == nullptr)
		{
			ImGui::TextWrapped("Root Bone not set'nt");
		}
		else {
			ImGui::Text("Root Bone: ");
			ImGui::SameLine();
			ImGui::Button(rootBone->name.c_str());
		}

		ImGui::Spacing();

		/*if (currentAnimation == nullptr) {
			ImGui::Text("Current Animation: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "None");
		}
		else {
			ImGui::Text("Current Animation: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", currentAnimation->name);
			ImGui::Text("Duration: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%.2f", currentAnimation->duration);
			ImGui::Text("Ticks per second: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%.2f", currentAnimation->ticksPerSec);
		}*/

		//List of existing animations
		static char newName[64];

		ImGui::Text("Select a new animation");
		for (int i = 0; i < GO_animations.size(); i++)
		{
			string animName = GO_animations[i]->name;

			if (currentAnimation == GO_animations[i]) {
				animName += " (Current)";
			}

			if (ImGui::Button(animName.c_str())) {

				for (int i = 0; i < GO_animations.size(); i++)
				{
					if (GO_animations[i]->name == animName) {
						PlayAnim(GO_animations[i]);
						time = 0.f;
						sprintf_s(newName, animName.c_str());
						break;
					}
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Remove Animation"))
			{
				DeleteAnimation(GO_animations[i]);
			}
		}

		if (currentAnimation != nullptr)
		{
			ImGui::InputText("Name", newName, IM_ARRAYSIZE(newName));
			ImGui::Checkbox("Loop", &currentAnimation->loop);

			ImGui::Separator();
		}

		ImGui::Text("Previous Animation Time: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%.2f", prevAnimationT);
		ImGui::Text("Current Animation Time: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", currentAnimationT);
		ImGui::Text("Blend Time: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%.2f", blendTime);

		ImGui::Spacing();
		if (playing)
		{
			ImGui::Text("Playing: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "true");
		}
		else
		{
			ImGui::Text("Playing: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "false");
		}

		ImGui::Spacing();

		//Table header
		ImGui::Columns(3, "Clips");
		ImGui::Separator();
		ImGui::Text("Clips");
		ImGui::NextColumn();
		ImGui::Text("Start");
		ImGui::NextColumn();
		ImGui::Text("End");
		ImGui::NextColumn();
		ImGui::Separator();

		for (size_t i = 0; i < clips.size(); i++)
		{
			if (ImGui::Button(GO_animations[i]->name.c_str())) {
				currentAnimation = GO_animations[i];
			}
			ImGui::NextColumn();
			ImGui::InputFloat("##start", &clips[i].startFrame, 0.0f, 0.0f, 0);
			ImGui::NextColumn();
			ImGui::InputFloat("##end", &clips[i].endFrame, 0.0f, 0.0f, 0);
			ImGui::NextColumn();
		}

		ImGui::Separator();
		ImGui::Columns(1);
		ImGui::Spacing();

		if (ImGui::Button("+"))
		{
			if (currentAnimation != nullptr) {
				AddClip(currentAnimation);
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("-"))
		{
			if (selectedClip != nullptr)
			{
				std::vector<AnimationClip> remainingClips;
				for (size_t i = 0; i < clips.size(); i++)
				{
					if (selectedClip != &clips[i])
						remainingClips.push_back(clips[i]);
				}

				selectedClip = nullptr;
				clips = remainingClips;
				remainingClips.clear();
			}
		}

		ImGui::Spacing();

		if (selectedClip != nullptr)
		{
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Text("Selected Clip");
			ImGui::InputText("Name", selectedClip->name, IM_ARRAYSIZE(selectedClip->name));
			ImGui::InputFloat("Start Frame", &selectedClip->startFrame, 1.0f, 0.0f);
			ImGui::InputFloat("End Frame", &selectedClip->endFrame, 1.0f, 0.0f);
			ImGui::Checkbox("Loop", &selectedClip->loop);
		}

		ImGui::Spacing();

		if (ImGui::Button("Apply")) {
			for (size_t i = 0; i < clips.size(); i++) {
				Animation* animation = ClipToAnim(clips[i]);
				AddAnimation(animation);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			selectedClip = nullptr;
			clips.clear();
		}

		ImGui::Spacing();

		// Delete ANIMATION Component

		bool deleteAnims = false;
		ImGui::Selectable("Delete Component", &deleteAnims);

		if (deleteAnims) {
			GO_animations.clear();
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

	if (rot.x > 360) {
		rot.x = 0;
	}
	else if (rot.x < 0) {
		rot.x = 360;
	}

	if (rot.y > 360) {
		rot.y = 0;
	}
	else if (rot.y < 0) {
		rot.y = 360;
	}

	if (rot.z > 360) {
		rot.z = 0;
	}
	else if (rot.z < 0) {
		rot.z = 360;
	}

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

	// Camera
	if (GO_camera != nullptr) {
		GO_camera->camFrustum.pos = float3(GO_trans.position.x, GO_trans.position.y, GO_trans.position.z);
	}

	SetGlobalMatrix();
}
void GameObject::UpdateRotation() {

	//Camera
	if (GO_camera != nullptr) {
		Quat dir;

		GO_camera->camFrustum.WorldMatrix().Decompose(float3(), dir, float3());

		dir = dir.FromEulerXYZ(math::DegToRad(GO_trans.rotation.x), math::DegToRad(GO_trans.rotation.y + 95), math::DegToRad(GO_trans.rotation.z));

		float4x4 mat = GO_camera->camFrustum.WorldMatrix();
		mat.SetRotatePart(dir.Normalized());

		GO_camera->camFrustum.SetWorldMatrix(mat.Float3x4Part());
	}

	SetGlobalMatrix();
}
void GameObject::UpdateScale() {
	SetGlobalMatrix();
}

void GameObject::UpdateTransform() {

	Transform globalTransform = GetGlobalTransform();

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
	mat4x4 translationMatrix, rotationMatrix, scaleMatrix;

	translationMatrix.translate(_position.x, _position.y, _position.z);

	float x = _rotation.x * DEGTORAD;
	float y = _rotation.y * DEGTORAD;
	float z = _rotation.z * DEGTORAD;

	rotationMatrix[0] = cos(y) * cos(z);
	rotationMatrix[1] = -cos(x) * sin(z) + sin(y) * cos(z) * sin(x);
	rotationMatrix[2] = sin(x) * sin(z) + sin(y) * cos(z) * cos(x);
	rotationMatrix[3] = 0;

	rotationMatrix[4] = cos(y) * sin(z);
	rotationMatrix[5] = cos(x) * cos(z) + sin(y) * sin(z) * sin(z);
	rotationMatrix[6] = -sin(x) * cos(z) + sin(y) * sin(z) * cos(x);
	rotationMatrix[7] = 0;

	rotationMatrix[8] = -sin(y);
	rotationMatrix[9] = cos(y) * sin(x);
	rotationMatrix[10] = cos(x) * cos(y);
	rotationMatrix[11] = 0;

	rotationMatrix[12] = 0;
	rotationMatrix[13] = 0;
	rotationMatrix[14] = 0;
	rotationMatrix[15] = 1;

	scaleMatrix.scale(_scale.x, _scale.y, _scale.z);

	GO_matrix = translationMatrix * rotationMatrix * scaleMatrix;

	if (parent != nullptr) {
		GO_matrix = GO_matrix * parent->GO_matrix;
	}

	for(int i = 0; i < childs.size(); i++){
		childs[i]->SetGlobalMatrix();
	}

	aabb.SetPos(float3(_position.x, _position.y, _position.z));
}
void GameObject::SetGlobalMatrix() {

	Transform gTrans = GetGlobalTransform();
	SetTransformMatrix(gTrans.position, gTrans.rotation, gTrans.scale);
}

Transform GameObject::GetGlobalTransform() {
	if (GetParent() == nullptr) return GO_trans;

	global_transform.position = GO_trans.position;
	global_transform.rotation = GO_trans.rotation;
	global_transform.scale = GO_trans.scale;

	return global_transform;
}

// MESH
void GameObject::AddMesh(Mesh* m) {
	GO_mesh = m;

	if (GO_mesh != nullptr) {
		GO_mesh->asignedGo = this;
	}

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

// Bounding Boxes (idk what the AA means)
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

// ANIMATIONS

void GameObject::AddAnimation(Animation* animation) {
	this->GO_animations.push_back(animation);
}
void GameObject::AddAnimation(std::vector<Animation*> animations)
{
	for (int i = 0; i < animations.size(); i++) {
		this->GO_animations.push_back(animations[i]);
	}
}

void GameObject::AddClip(Animation* animation) {
	if (animation != nullptr) {
		AnimationClip clip;

		strcpy(clip.name, animation->name.c_str());
		clip.startFrame = animation->initTimeAnim;
		clip.endFrame = animation->initTimeAnim + animation->duration;
		clip.originalAnimation = animation;

		clips.push_back(clip);
	}
}

void GameObject::StartAnimation() {
	if (rootBone == nullptr) {
		if (!bones.empty()) {
			rootBone == bones[0];
		}
		else {
			FindRootBone();
			return;
		}
	}

	if (GO_animations.size() > 0)
	{
		if (currentAnimation == nullptr)
		{
			PlayAnim(GO_animations[0]);
		}
	}
}

void GameObject::UpdateAnimation(float dt) {

	// Update Current Animation
	if (this->playing) {
		if (!started) { StartAnimation(); }
		else {
			if (currentAnimation != nullptr) {
				
				//Updating animation blend // Este comentario tiene ya 3 generaciones
				float blendRatio = 0.0f;
				if (blendTimeDuration > 0.0f)
				{
					prevAnimationT += dt;
					blendTime += dt;

					if (blendTime >= blendTimeDuration)
					{
						blendTimeDuration = 0.0f;
					}
					else if (prevAnimation && prevAnimationT >= prevAnimation->duration)
					{
						if (prevAnimation->loop == true)
						{
							prevAnimationT = 0.0f;
						}
					}

					if (blendTimeDuration > 0.0f)
						blendRatio = blendTime / blendTimeDuration;
				}
				//Endof Updating animation blend // Este comentario tiene ya 3 generaciones

				time += dt;

				currentAnimationT = dt * currentAnimation->ticksPerSec;
				currentAnimationT += currentAnimation->initTimeAnim;
				if (currentAnimation->loop == true) {
					time = 0.0f;
				}

				UpdateChannelsTransform(currentAnimation, blendRatio > 0.0f ? prevAnimation : nullptr, blendRatio);
			}
		}
	}

	// Draw bones if needed
	if (drawBones && rootBone != nullptr) {
		DrawBones(rootBone);
	}
}

void GameObject::DrawBones(GameObject* p) 
{
	if (!p->childs.empty()) {
		for (int i = 0; i < p->childs.size(); i++) {
			p->childs[i]->DrawBones(p->childs[i]);

			std::vector<float3> line;

			line.push_back(float3(p->GetPos().x, p->GetPos().y, p->GetPos().z));
			line.push_back(float3(p->childs[i]->GetPos().x, p->childs[i]->GetPos().y, p->childs[i]->GetPos().z));

			for (int j = 0; j < line.size(); j++) {
				app->scene->AddLines(line[j], Magenta);
			}
		}
	}
}

bool GameObject::FindRootBone()
{
	bool ret = true;
	if (rootBoneID != 0)
	{
		for (int i = 0; i < app->editor->gameObjects.size(); i++) {
			if (app->editor->gameObjects[i]->ID = rootBoneID) {
				rootBone = app->editor->gameObjects[i];
			}
		}

		if (rootBone == nullptr)
		{
			rootBoneID = 0;
			ret = false;
		}
		else
		{
			bones.clear();
			StoreBoneMapping(rootBone);
		}

		if (GO_mesh != nullptr) {
			if (GO_mesh->rootBone != nullptr) {
				rootBone = GO_mesh->rootBone;
			} 
			/*GO_mesh->SetRootBone(rootBone);*/ // ?¿
		}
	}

	return ret;
}

void GameObject::StoreBoneMapping(GameObject* go) {
	bones[go->name] = go;

	for (int i = 0; i < go->childs.size(); i++)
	{
		StoreBoneMapping(go->childs[i]);
	}
}

void GameObject::UpdateChannelsTransform(const Animation* animationPlaying, const Animation* blend, float blendRatio) 
{
	uint currentFrame = currentAnimationT;
	uint prevBlendFrame = 0;

	if (blend != nullptr) {
		prevBlendFrame = (blend->ticksPerSec * prevAnimationT) + blend->initTimeAnim;
	}

	std::map<GameObject*, Channel*>::iterator boneIt;
	for (boneIt = bonesCurrentAnim.begin(); boneIt != bonesCurrentAnim.end(); ++boneIt)
	{
		Channel& channel = *boneIt->second;

		float3 position = GetChannelPosition(channel, currentFrame, float3(boneIt->first->GetPos().x, boneIt->first->GetPos().y, boneIt->first->GetPos().z));
		float3 rotation = GetChannelRotation(channel, currentFrame, float3(boneIt->first->GetRot().x, boneIt->first->GetRot().y, boneIt->first->GetRot().z));
		float3 scale = GetChannelScale(channel, currentFrame, float3(boneIt->first->GetScale().x, boneIt->first->GetScale().y, boneIt->first->GetScale().z));

		// BLEND S
		if (blend != nullptr)
		{
			std::map<GameObject*, Channel*>::iterator foundChannel = bonesPrevAnim.find(boneIt->first);
			if (foundChannel != bonesPrevAnim.end()) {
				const Channel& blendChannel = *foundChannel->second;

				position = float3::Lerp(GetChannelPosition(blendChannel, prevBlendFrame, float3(boneIt->first->GetPos().x, boneIt->first->GetPos().y, boneIt->first->GetPos().z)), position, blendRatio);
				rotation = float3::Lerp(GetChannelRotation(blendChannel, prevBlendFrame, float3(boneIt->first->GetRot().x, boneIt->first->GetRot().y, boneIt->first->GetRot().z)), rotation, blendRatio);
				scale = float3::Lerp(GetChannelScale(blendChannel, prevBlendFrame, float3(boneIt->first->GetScale().x, boneIt->first->GetScale().y, boneIt->first->GetScale().z)), scale, blendRatio);
			}
		}

		boneIt->first->SetTransform(vec3(position.x, position.y, position.z), vec3(rotation.x, rotation.y, rotation.z), vec3(scale.x, scale.y, scale.z));
	}
}

float3	GameObject::GetChannelPosition(const Channel& ch, float currentKey, float3 defPos) const {
	if (ch.posKeys.size() > 0)
	{
		std::map<double, float3>::const_iterator previous = ch.GetPrevPosKey(currentKey);
		std::map<double, float3>::const_iterator next = ch.GetNextPosKey(currentKey);

		if (ch.posKeys.begin()->first == -1) {
			return defPos;
		}

		// Check Blending Ratio between Keys
		if (previous == next) {
			defPos = previous->second;
		}
		else
		{
			float ratio = (currentKey - previous->first) / (next->first - previous->first);
			defPos = previous->second.Lerp(next->second, ratio);
		}
	}

	return defPos;
}

float3	GameObject::GetChannelRotation(const Channel& ch, float currentKey, float3 defRot) const {
	if (ch.posKeys.size() > 0)
	{
		std::map<double, float3>::const_iterator previous = ch.GetPrevRotKey(currentKey);
		std::map<double, float3>::const_iterator next = ch.GetNextRotKey(currentKey);

		if (ch.posKeys.begin()->first == -1) {
			return defRot;
		}

		// Check Blending Ratio between Keys
		if (previous == next) {
			defRot = previous->second;
		}
		else
		{
			float ratio = (currentKey - previous->first) / (next->first - previous->first);
			defRot = previous->second.Lerp(next->second, ratio);
		}
	}

	return defRot;
}

float3	GameObject::GetChannelScale(const Channel& ch, float currentKey, float3 defScale) const {
	if (ch.scaleKeys.size() > 0)
	{
		std::map<double, float3>::const_iterator previous = ch.GetPrevScaleKey(currentKey);
		std::map<double, float3>::const_iterator next = ch.GetPrevScaleKey(currentKey);

		if (ch.scaleKeys.begin()->first == -1) {
			return defScale;
		}

		// Check Blending Ratio between Keys
		if (previous == next)
		{
			defScale = previous->second;
		}
		else
		{
			float ratio = (currentKey - previous->first) / (next->first - previous->first);
			defScale = previous->second.Lerp(next->second, ratio);
		}
	}
	return defScale;
}

void GameObject::DeleteAnimation(Animation* anim) {
	for (int i = 0; i < GO_animations.size(); i++) {
		if (GO_animations[i] == anim) {
			GO_animations.erase(GO_animations.begin() + i);
		}
	}
}

void GameObject::PlayAnim(Animation* anim, float blendDuration, float Speed){

	prevAnimation = currentAnimation;
	prevAnimationT = time;
	currentAnimation = anim;
	blendTimeDuration = blendDuration;
	blendTime = 0.0f;
	time = 0;
	this->speed = speed;

	SetAnimationChannelToBones(currentAnimation, bonesCurrentAnim);
	SetAnimationChannelToBones(prevAnimation, bonesPrevAnim);
}

void GameObject::PauseAnim() {
	playing = false;
}

void GameObject::ResumeAnim() {
	playing = true;
}

AnimationClip::AnimationClip() : name("Namen't"), startFrame(0), endFrame(0), originalAnimation(nullptr), loop(false) {

}

Animation* GameObject::ClipToAnim(AnimationClip clip)
{
	Animation* animation = new Animation(clip.name, clip.endFrame - clip.startFrame, clip.originalAnimation->ticksPerSec);

	animation->initTimeAnim = clip.startFrame;
	animation->loop = clip.loop;

	return animation;
}

void GameObject::SetAnimationChannelToBones(Animation* animation, std::map<GameObject*, Channel*>& lookUpTable)
{
	if (animation == nullptr)
		return;

	lookUpTable.clear();

	std::map<std::string, GameObject*>::iterator boneIt = bones.begin();
	for (boneIt; boneIt != bones.end(); ++boneIt)
	{
		std::map<std::string, Channel>::iterator channelIt = animation->channels.find(boneIt->first);

		if (channelIt != animation->channels.end())
		{
			lookUpTable[boneIt->second] = &channelIt->second;
		}
	}
}