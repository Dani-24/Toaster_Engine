#pragma once

#include "Globals.h"
#include "glmath.h"

#include "../External/MathGeoLib/include/Geometry/Frustum.h"

#include "Buffer.h"

class Camera {
public:

	Camera();
	~Camera();

	void UpdateCamera(float dt);

	void Look(Frustum& position, const float3& Reference, bool RotateAroundReference = false);
	void LookAt(const float3& Spot);

	void FocusCam(float3 center, float offset);

	void Move(const float3& Movement);
	float4x4 GetViewMatrix();
	vec3 GetPos();

	void SetAspectRatio(float aspectRatio);
	float GetAspectRatio() { return aspectRatio; }

	void SetFOV(float fov);
	float GetFOV() { return FOV; };

	void SetRange(float range);
	float GetRange() { return range; };

	void DebugDraw();

	void EditorCameraControl(float dt);

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
	float range = 1000.0f;

	bool debugDraw = false;

	Buffer cameraBuffer;
};
