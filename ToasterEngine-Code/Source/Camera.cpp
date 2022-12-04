#include "Camera.h"
#include "Application.h"

#include "ModuleEditor.h"

Camera::Camera(float3 pos, float3 lookat, bool isEditor) {
	camFrustum.pos = pos;

	if (!isEditor) {
		camFrustum.type = math::FrustumType::PerspectiveFrustum;

		float3 X = float3(1.0f, 0.0f, 0.0f);
		float3 Y = float3(0.0f, 1.0f, 0.0f);
		float3 Z = float3(0.0f, 0.0f, 1.0f);

		camFrustum.verticalFov = FOV = math::DegToRad(60.0f);
		aspectRatio = 1.7f;
		camFrustum.horizontalFov = 2.0f * atanf(tanf(camFrustum.verticalFov / 2.0f) * aspectRatio);

		camFrustum.nearPlaneDistance = 0.01f;
		camFrustum.farPlaneDistance = range;

		camFrustum.front = Z;
		camFrustum.up = Y;
	}
	else {
		camFrustum.type = math::FrustumType::PerspectiveFrustum;
		camFrustum.nearPlaneDistance = 0.01f;
		camFrustum.farPlaneDistance = 9999;

		camFrustum.verticalFov = FOV = math::DegToRad(60.0f);
		aspectRatio = 1.7f;
		camFrustum.horizontalFov = 2.0f * atanf(tanf(camFrustum.verticalFov / 2.0f) * aspectRatio);
	}
	LookAt(lookat);
	active = false;
}

Camera::~Camera() {

}

void Camera::UpdateCamera(float dt)
{
	if (this == app->camera->activeCamera) {
		active = true;
	}
	else {
		active = false;
	}

	if (app->editor->selectedGameObj != nullptr) {
		if (this == app->editor->selectedGameObj->GO_camera || app->editor->showAllAABB) {
			DebugDraw();
		}
	}
}

void Camera::Look(Frustum& Position, const float3& Reference, bool RotateAroundReference)
{
	Position.front = (Reference - Position.pos).Normalized();
	float3 X = float3(0, 1, 0).Cross(Position.front).Normalized();
	Position.up = Position.front.Cross(X);
}

void Camera::LookAt(const float3& Spot)
{
	camFrustum.front = (Spot - camFrustum.pos).Normalized();
	float3 X = float3(0, 1, 0).Cross(camFrustum.front).Normalized();
	camFrustum.up = camFrustum.front.Cross(X);
}

void Camera::Move(const float3& Movement)
{
	camFrustum.pos += Movement;
}

float4x4 Camera::GetViewMatrix()
{
	math::float4x4 matrix;

	matrix = camFrustum.ViewMatrix();

	return matrix.Transposed();
}

vec3 Camera::GetPos()
{
	return vec3(camFrustum.pos.x, camFrustum.pos.y, camFrustum.pos.z);
}

void Camera::FocusCam(float3 center, float offset) {
	LookAt(center);
	camFrustum.pos = center + (((camFrustum.pos - center).Normalized()) * offset);
}

void Camera::SetAspectRatio(float aspectRatio)
{
	this->aspectRatio = aspectRatio;
	camFrustum.horizontalFov = 2.f * atanf(tanf(camFrustum.verticalFov * 0.5f) * aspectRatio);
}

void Camera::SetFOV(float fov)
{
	camFrustum.verticalFov = FOV = math::DegToRad(fov);
	camFrustum.horizontalFov = 2.0f * atanf(tanf(camFrustum.verticalFov / 2.0f) * aspectRatio);
}

void Camera::SetRange(float range)
{
	this->range = range;
	camFrustum.farPlaneDistance = range;
}

void Camera::DebugDraw() {

	float3 corners[8];
	float3 frustum_corners[8];

	// Get Frustum corners
	corners[0] = camFrustum.CornerPoint(0) - camFrustum.pos;
	corners[1] = camFrustum.CornerPoint(2) - camFrustum.pos;
	corners[2] = camFrustum.CornerPoint(4) - camFrustum.pos;
	corners[3] = camFrustum.CornerPoint(6) - camFrustum.pos;
	corners[4] = camFrustum.CornerPoint(1) - camFrustum.pos;
	corners[5] = camFrustum.CornerPoint(3) - camFrustum.pos;
	corners[6] = camFrustum.CornerPoint(5) - camFrustum.pos;
	corners[7] = camFrustum.CornerPoint(7) - camFrustum.pos;

	// Normalise corners to get a -1, 0, 1 vector
	frustum_corners[0] = (corners[4] - corners[0]).Normalized() + camFrustum.pos;
	frustum_corners[1] = (corners[5] - corners[1]).Normalized() + camFrustum.pos;
	frustum_corners[2] = (corners[6] - corners[2]).Normalized() + camFrustum.pos;
	frustum_corners[3] = (corners[7] - corners[3]).Normalized() + camFrustum.pos;
	frustum_corners[4] = (corners[4] - corners[0]).Normalized() * 10 + camFrustum.pos;
	frustum_corners[5] = (corners[5] - corners[1]).Normalized() * 10 + camFrustum.pos;
	frustum_corners[6] = (corners[6] - corners[2]).Normalized() * 10 + camFrustum.pos;
	frustum_corners[7] = (corners[7] - corners[3]).Normalized() * 10 + camFrustum.pos;

	std::vector<float3> frustum_lines;
	// Near Plane lines
	frustum_lines.push_back(frustum_corners[0]);
	frustum_lines.push_back(frustum_corners[1]);
	frustum_lines.push_back(frustum_corners[0]);
	frustum_lines.push_back(frustum_corners[2]);
	frustum_lines.push_back(frustum_corners[1]);
	frustum_lines.push_back(frustum_corners[3]);
	frustum_lines.push_back(frustum_corners[2]);
	frustum_lines.push_back(frustum_corners[3]);

	// Far Plane lines
	corners[0] += camFrustum.pos;
	corners[1] += camFrustum.pos;
	corners[2] += camFrustum.pos;
	corners[3] += camFrustum.pos;
	corners[4] += camFrustum.pos;
	corners[5] += camFrustum.pos;
	corners[6] += camFrustum.pos;
	corners[7] += camFrustum.pos;

	frustum_lines.push_back(corners[4]);
	frustum_lines.push_back(corners[5]);
	frustum_lines.push_back(corners[4]);
	frustum_lines.push_back(corners[6]);
	frustum_lines.push_back(corners[5]);
	frustum_lines.push_back(corners[7]);
	frustum_lines.push_back(corners[6]);
	frustum_lines.push_back(corners[7]);

	// Lines between Far & Near planes
	frustum_lines.push_back(frustum_corners[0]);
	frustum_lines.push_back(corners[4]);
	frustum_lines.push_back(frustum_corners[1]);
	frustum_lines.push_back(corners[5]);
	frustum_lines.push_back(frustum_corners[2]);
	frustum_lines.push_back(corners[6]);
	frustum_lines.push_back(frustum_corners[3]);
	frustum_lines.push_back(corners[7]);

	// Add Lines to the DrawLines queue
	for (int i = 0; i < frustum_lines.size(); i++) {
		app->scene->AddLines(frustum_lines[i], Green);
	}

}

bool Camera::FrustumCulling(GameObject* go) {
	bool canRender = false;
	if (frustumCulling) {
		if (camFrustum.Contains(go->aabb)) {
			canRender = true;
		}
	}
	else {
		return true;
	}

	return canRender;
}

void Camera::EditorCameraControl(float dt) {

	if (app->editor->selectedGameObj != nullptr) {
		camFocusPos = float3(app->editor->selectedGameObj->GetPos().x, app->editor->selectedGameObj->GetPos().y, app->editor->selectedGameObj->GetPos().z);
	}
	else {
		camFocusPos = float3(0, 0, 0);
	}

	float speed;
	if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		speed = camSpeed * 2 * dt;
	}
	else {
		speed = camSpeed * dt;
	}

	// Camera Focus
	if (app->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) {
		focusDist = camFrustum.pos.Distance(camFocusPos);

		FocusCam(camFocusPos, focusDist);

		camOrbitalPos = camFocusPos;
	}

	float3 newPos(0, 0, 0);

	// Zoom with mouse wheel
	if (app->input->GetMouseZ() != 0)
	{
		newPos += camFrustum.front * app->input->GetMouseZ() * speed * 4;
	}

	// Movement Right Click + WASD
	// Camera forwards
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += camFrustum.front * speed;
	// camera backwards
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= camFrustum.front * speed;
	// move horizontal left
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= camFrustum.WorldRight() * speed;
	// move horizontal right
	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += camFrustum.WorldRight() * speed;
	// Camera up
	if (app->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos.y += speed;
	// camera down
	if (app->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos.y -= speed;

	camFrustum.pos += newPos;
	camFocusPos += newPos;
	camOrbitalPos += newPos;

	// Orbit by Left Click
	if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		int dx = -app->input->GetMouseXMotion();
		int dy = -app->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		Quat dir;

		camFrustum.WorldMatrix().Decompose(float3(), dir, float3());

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			Quat X = Quat::identity;
			X.SetFromAxisAngle(float3(0, 1, 0), DeltaX * DEGTORAD);

			dir = dir * X;
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Quat Y = Quat::identity;
			Y.SetFromAxisAngle(float3(1, 0, 0), DeltaY * DEGTORAD);

			dir = dir * Y;
		}

		float4x4 mat = camFrustum.WorldMatrix();
		mat.SetRotatePart(dir.Normalized());
		camFrustum.SetWorldMatrix(mat.Float3x4Part());

		float3 orbitPoint = camOrbitalPos;

		camFrustum.pos = orbitPoint + camFrustum.front * -camFrustum.pos.Distance(orbitPoint);
		LookAt(orbitPoint);
	}
}

void Camera::CalculateMousePicking()
{
	float mouseX = (((float)app->input->GetMouseX() - app->editor->editorPos.x) / app->editor->editorSize.x) - 0.5f;
	float mouseY = (((float)app->input->GetMouseY() - app->editor->editorPos.y) / app->editor->editorSize.y) - 0.5f;

	LineSegment raycast = camFrustum.UnProjectLineSegment(mouseX * 2, -mouseY * 2);

	std::vector<GO_Hitted> go_hits;

	for (int i = 0; i < app->editor->gameObjects.size(); i++)
	{
		if (raycast.Intersects(app->editor->gameObjects[i]->aabb))
		{
			LOG("HIT");
			GO_Hitted go;

			go.gameObject = app->editor->gameObjects[i];
			go.distance = Sqrt((this->GetPos().x - app->editor->gameObjects[i]->GetPos().x) * (this->GetPos().x - app->editor->gameObjects[i]->GetPos().x) +
				(this->GetPos().y - app->editor->gameObjects[i]->GetPos().y) * (this->GetPos().y - app->editor->gameObjects[i]->GetPos().y) +
				(this->GetPos().z - app->editor->gameObjects[i]->GetPos().z) * (this->GetPos().z - app->editor->gameObjects[i]->GetPos().z));

			go_hits.push_back(go);
		}
	}

	// Return Nullptr is no gameobject was selected
	if (go_hits.empty()) {
		app->editor->SetSelectedGameObject(nullptr);
		return;
	}

	// Check the closest gameobject
	float closest = 100000;
	for (int i = 0; i < go_hits.size(); i++) {
		if (go_hits[i].distance < closest) {
			closest = go_hits[i].distance;
		}
	}

	// Set closest gameobject selected
	for (int i = 0; i < go_hits.size(); i++) {
		if(closest == go_hits[i].distance)
		app->editor->SetSelectedGameObject(go_hits[i].gameObject);
		break;
	}
}