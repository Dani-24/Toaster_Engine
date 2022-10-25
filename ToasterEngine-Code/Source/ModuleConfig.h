#pragma once
#include "Module.h"
#include "Globals.h"

class ModuleConfig : public Module
{
public:
	ModuleConfig(Application* app, bool start_enabled = true);
	~ModuleConfig();

	bool Start();

	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	bool CleanUp();

private:

};
