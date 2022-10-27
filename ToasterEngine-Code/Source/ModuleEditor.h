#pragma once
#include "Module.h"
#include "Globals.h"
#include "../External/ImGui/imgui.h"

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

	float cooldown = 100, maxCooldown = 100, minCooldown = 0;

	bool exit = false;
	bool showingEditor = true;

	void AreYouSureAboutThat(bool *open);

	void ShowConfigMenu(bool* open);
	void ShowInspectorMenu(bool* open);
	void ShowHierarchyMenu(bool* open);
	void ShowConsoleMenu(bool* open);
	void ShowAboutMenu(bool* open);
	void ShowGameEditorWindow(bool* open);
	void ShowConfiguration(bool* open);

	std::vector<std::string> logs;

public:
	void AddLogMsg(const char* msg);
};