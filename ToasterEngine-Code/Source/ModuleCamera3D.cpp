#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("TOASTER: Setting up the cameras");
	bool ret = true;

	// Create Editor Camera
	AddCamera(editorCamera);

	// Create Scene Main Camera

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("TOASTER: Cleaning cameras");

	cameras.clear();

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	
	editorCamera.EditorCameraControl(dt);

	for (int i = 0; i < cameras.size(); i++) {
		cameras[i].UpdateCamera(dt);
	}

	return UPDATE_CONTINUE;
}

void ModuleCamera3D::AddCamera(Camera newCam) {
	newCam.ID = camerasID;
	cameras.push_back(newCam);
}

void ModuleCamera3D::DeleteCamera(Camera cam) {
	for (int i = 0; i < cameras.size(); i++) {
		if (cameras[i].ID == cam.ID) {
			cameras.erase(cameras.begin() + i);
		}
	}
}