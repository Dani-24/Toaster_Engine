#pragma once
#include "Module.h"
#include "Globals.h"

#include "Camera.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void AddCamera(Camera newCam);
	void DeleteCamera(Camera cam);

public:
	Camera editorCamera;
	std::vector<Camera> cameras;
	uint camerasID = 0;
};