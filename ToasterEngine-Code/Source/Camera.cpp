#include "Camera.h"
#include "Application.h"

Camera::Camera(float3 pos, float3 lookat) {
	camFrustum.pos = pos;
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

	if (debugDraw) {
		DebugDraw();
	}
}

// -----------------------------------------------------------------
void Camera::Look(Frustum& Position, const float3& Reference, bool RotateAroundReference)
{
	Position.front = (Reference - Position.pos).Normalized();
	float3 X = float3(0, 1, 0).Cross(Position.front).Normalized();
	Position.up = Position.front.Cross(X);
}

// -----------------------------------------------------------------
void Camera::LookAt(const float3& Spot)
{
	camFrustum.front = (Spot - camFrustum.pos).Normalized();
	float3 X = float3(0, 1, 0).Cross(camFrustum.front).Normalized();
	camFrustum.up = camFrustum.front.Cross(X);
}


// -----------------------------------------------------------------
void Camera::Move(const float3& Movement)
{
	camFrustum.pos += Movement;
}

// -----------------------------------------------------------------
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
	camFrustum.verticalFov = FOV = fov;
	camFrustum.horizontalFov = 2.0f * atanf(tanf(camFrustum.verticalFov / 2.0f) * aspectRatio);
}

void Camera::SetRange(float range)
{
	this->range = range;
	camFrustum.farPlaneDistance = range;
}

void Camera::DebugDraw() {

	// Draw the frustum

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