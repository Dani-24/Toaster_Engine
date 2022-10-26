#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

#include"../External/MathGeoLib/include/Geometry/Frustum.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(Frustum& position, const float3&Reference, bool RotateAroundReference = false);
	void LookAt(const float3 &Spot);

	void FocusCam(float3 center, float offset);

	void Move(const float3 &Movement);
	float4x4 GetViewMatrix();
	vec3 GetPos();
private:

	void CalculateViewMatrix();

public:
	
	Frustum camFrustum;

	//vec3 X, Y, Z, Position, Reference;

	bool freeCamera;
	bool finish;
	bool firstPerson;

	float camSpeed = 25.0f;

	bool psychoControls = true;
	bool mouseOnCamera = true;

private:

	//mat4x4 ViewMatrix, ViewMatrixInverse;
};