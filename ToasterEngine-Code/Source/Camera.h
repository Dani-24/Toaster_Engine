#pragma once

#include "Globals.h"
#include "glmath.h"

#include "../External/MathGeoLib/include/Geometry/Frustum.h"

#include "Buffer.h"
#include "Primitive.h"

class GameObject;

class Camera {
public:

	Camera(float3 pos = float3(110.0f, 30.0f, 0.0f), float3 LookAt = float3(0.0f, 0.0f, 0.0f), bool isEditor = false);
	~Camera();

	void UpdateCamera(float dt);

	void Look(Frustum& position, const float3& Reference, bool RotateAroundReference = false);
	void LookAt(const float3& Spot);

	void FocusCam(float3 center, float offset);

	void Move(const float3& Movement);
	float4x4 GetViewMatrix();
	vec3 GetPos();

	void SetAspectRatio(float aspectRatio);
	void SetFOV(float fov);
	void SetRange(float range);

	void DebugDraw();

	void EditorCameraControl(float dt);

	// Return if the GameObject is inside the camera frustum
	bool FrustumCulling(GameObject* go);

	// Mouse Picking
	void CalculateMousePicking();

public:

	Frustum camFrustum;

	uint ID;

	float camSpeed = 15.0f;

	bool mouseOnCamera = true;

	float3 camFocusPos = float3(0, 0, 0);
	float focusDist = 0;

	float3 camOrbitalPos = float3(0, 0, 0);

	float aspectRatio = 1.7f;
	float FOV = 60.0f;
	float range = 200.0f;

	bool debugDraw = false;

	Buffer cameraBuffer;

	bool active;
	bool frustumCulling = true;
};

struct GO_Hitted
{
	GameObject* gameObject;
	float distance;
};