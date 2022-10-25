#pragma once
#include "Module.h"
#include "Globals.h"
#include "Primitive.h"

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

private:


public:
	bool axis = true;

};
