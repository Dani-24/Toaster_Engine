#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"

#include "GameObject.h"

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

	// Editor Camera
	editorCamera = new Camera();

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
	
	editorCamera->EditorCameraControl(dt);

	for (int i = 0; i < cameras.size(); i++) {
		cameras[i]->UpdateCamera(dt);
	}

	return UPDATE_CONTINUE;
}

void ModuleCamera3D::AddCamera(Camera* newCam) {

	GameObject* cameraObject = new GameObject("New Camera", app->editor->root, newCam);
	app->editor->AddGameObject(cameraObject);

	newCam->ID = camerasID;
	cameras.push_back(newCam);

	activeCamera = newCam;
}

void ModuleCamera3D::DeleteCamera(Camera* cam) {
	for (int i = 0; i < cameras.size(); i++) {
		if (cameras[i]->ID == cam->ID) {
			cameras.erase(cameras.begin() + i);
		}
	}
}

Camera* ModuleCamera3D::CreateCamera() {
	Camera* newCam = new Camera();
	return newCam;
}