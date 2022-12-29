#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleMesh3D.h"
#include "ModuleImporter.h"
#include "ModuleTexture.h"
#include "ModuleAnimation.h"

#include "GameObject.h"

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleScene* scene;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEditor* editor;
	ModuleMesh3D* mesh3d;
	ModuleAnimation* anim3d;
	ModuleImporter* importer;
	ModuleTexture* textures;

private:

	Timer	ms_timer;
	float	dt;
	std::vector<Module*> list_modules;

public:
	int maxFps = 60;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	bool stopLogging = false;

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* appLog;
extern Application* app;