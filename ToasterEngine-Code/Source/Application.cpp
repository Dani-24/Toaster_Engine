#include "Application.h"

extern Application* appLog = nullptr;
Application* app = NULL;

Application::Application()
{
	appLog = this;
	app = this;

	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	scene = new ModuleScene(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	editor = new ModuleEditor(this);
	mesh3d = new ModuleMesh3D(this);
	importer = new ModuleImporter(this);
	textures = new ModuleTexture(this);

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);

	AddModule(importer);
	
	// Scenes

	AddModule(editor);

	AddModule(scene);

	AddModule(mesh3d);
	AddModule(textures);


	// Renderer last!
	AddModule(renderer3D);

	
}

Application::~Application()
{
	for (int i = list_modules.size() - 1; i >= 0; i--) {
		delete list_modules[i];
		list_modules[i] = nullptr;
	}

	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	for (uint i = 0; i < list_modules.size() && ret == true; i++) {
		ret = list_modules[i]->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Startooooooooooo");
	
	for (uint i = 0; i < list_modules.size() && ret == true; i++) {
		list_modules[i]->Start();
	}
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	float maxdt = 1.f / maxFps;

	if (dt < maxdt)
	{
		
		float difDt = (maxdt - dt)*1000;
		SDL_Delay(difDt);
		
		dt = maxdt;
	}
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	// cual era el objetivo de p2List y p2Point si el pto visual tiene funciones que hacen lo mismo y mejor

	for (uint i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++) {
		ret = list_modules[i]->PreUpdate(dt);
	}

	for (uint i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++) {
		ret = list_modules[i]->Update(dt);
	}

	for (uint i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++) {
		ret = list_modules[i]->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	stopLogging = true;

	for (uint i = list_modules.size() - 1; i > -1 && ret == true; i--) {
		ret = list_modules[i]->CleanUp();
	}

	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}