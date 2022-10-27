#pragma once
#include "Module.h"
#include "Globals.h"
#include "../External/ImGui/imgui.h"

#define MAX_LOGS_SIZE 100

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
	void ShowInspectorMenu(bool* open);
	void ShowHierarchyMenu(bool* open);
	void ShowConsoleMenu(bool* open);
	void ShowAboutMenu(bool* open);
	void ShowGameEditorWindow(bool* open);
	void ShowConfiguration(bool* open);
	void ShowAssetManager(bool* open);
	void ShowAssetExplorer(bool* open);

	// Logs
	std::vector<std::string> logs;

	std::vector<float> fpslog;
	std::vector<float> mslog;
	std::vector<float> memlog;

	void Space();

public:
	void AddLogMsg(const char* msg);
};