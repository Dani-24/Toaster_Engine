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
	editorCamera = new Camera(float3(20.0f, 5.0f, 20.0f), float3(0, 0, 0), true);

	editorCamera->cameraBuffer.SetBufferInfo();
	editorCamera->cameraBuffer.SetBufferDimensions(app->window->width, app->window->height);

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
	if (renewBuffer == true) 
	{
		editorCamera->RegenBuffer(renewW, renewH);

		for (int i = 0; i < cameras.size(); i++) {
			cameras[i]->RegenBuffer(renewW, renewH);
		}

		renewBuffer = false;
	}
	
	if (moveEditCam) {
		editorCamera->EditorCameraControl(dt);
	}

	for (int i = 0; i < cameras.size(); i++) {
		cameras[i]->UpdateCamera(dt);
	}

	return UPDATE_CONTINUE;
}

void ModuleCamera3D::AddCamera(Camera* newCam, std::string name) {

	GameObject* cameraObject = new GameObject(name.c_str(), app->editor->root);
	app->editor->AddGameObject(cameraObject);

	cameraObject->AddComponent(Component::TYPE::CAMERA); // AQUI FALTA ASIGNAR ESA CAMARA AL GO

	newCam->ID = camerasID;
	cameras.push_back(newCam);

	newCam->cameraBuffer.SetBufferInfo();
	newCam->cameraBuffer.SetBufferDimensions(app->window->width, app->window->height);

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

void ModuleCamera3D::RenewCameraBuffers(int w, int h) {
	renewBuffer = true;
	renewW = w;
	renewH = h;
}