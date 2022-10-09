#pragma once
#include "Module.h"
#include "Globals.h"
#include "ImGui/imgui.h"

class ModuleEditor : public Module 
{
public:

	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void Draw();

private:

};
