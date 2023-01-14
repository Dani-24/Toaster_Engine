#pragma once
#include "Component.h"

#include "ModuleCamera3D.h"
#include "Camera.h"

class C_Camera : public Component
{
public:
	C_Camera();
	C_Camera(GameObject* _gm);
	virtual ~C_Camera();

	bool OnEditor() override;

	Camera* GetCamera();

public:

	Camera* GO_camera;

	float3 camLookAt = float3(0.0f, 1.0f, 0.0f);
};