#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	camFrustum.pos = float3(6.0f, 3.5f, 1.5f);
	LookAt(float3(0, 0, 0));
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// if(mouseOnCamera){ //Only move camera out of the UI , WIP } 

	float3 newPos(0, 0, 0);
	float speed = camSpeed * dt;
	if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		speed = camSpeed * 2 * dt;
	}

	if (app->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) {
		FocusCam(float3(0,0,0), 6);

		// This should focus 0,0,0 if there is no GameObject or the GameObject if it exists
	}

	// Zoom with mouse wheel
	if (app->input->GetMouseZ() != 0)
	{
		newPos += camFrustum.front * app->input->GetMouseZ() * speed;
	}

	// Movement Right Click + WASD
	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT || !psychoControls)
	{
		// Camera up
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos.y += speed;
		// camera down
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos.y -= speed;
		// move horizontal left
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= camFrustum.WorldRight() * speed;
		// move horizontal right
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += camFrustum.WorldRight() * speed;

	}

	camFrustum.pos += newPos;
	camFocusPos += newPos;

	// Orbit by Left Click + Alt
	if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT || !psychoControls && app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
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

		float3 orbitPoint = camFocusPos;

		camFrustum.pos = orbitPoint + camFrustum.front * -camFrustum.pos.Distance(orbitPoint);
		LookAt(orbitPoint);
	}

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(Frustum& Position, const float3& Reference, bool RotateAroundReference)
{
	Position.front = (Reference - Position.pos).Normalized();
	float3 X = float3(0, 1, 0).Cross(Position.front).Normalized();
	Position.up = Position.front.Cross(X);
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const float3& Spot)
{
	camFrustum.front = (Spot - camFrustum.pos).Normalized();
	float3 X = float3(0, 1, 0).Cross(camFrustum.front).Normalized();
	camFrustum.up = camFrustum.front.Cross(X);
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3& Movement)
{
	camFrustum.pos += Movement;
}

// -----------------------------------------------------------------
float4x4 ModuleCamera3D::GetViewMatrix()
{
	math::float4x4 matrix;

	matrix = camFrustum.ViewMatrix();

	return matrix.Transposed();
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	/*ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);*/
}

vec3 ModuleCamera3D::GetPos() 
{
	return vec3(camFrustum.pos.x, camFrustum.pos.y, camFrustum.pos.z);
}

void ModuleCamera3D::FocusCam(float3 center, float offset) {
	LookAt(center);
	camFrustum.pos = center + (((camFrustum.pos - center).Normalized()) * offset);
}