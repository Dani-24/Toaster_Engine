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

	void AddCamera(Camera* newCam, std::string name);
	void DeleteCamera(Camera* cam);

	Camera* CreateCamera();

public:
	Camera* editorCamera;
	bool moveEditCam = false;

	std::vector<Camera*> cameras;
	uint camerasID = 0;

	Camera* activeCamera = nullptr;
};