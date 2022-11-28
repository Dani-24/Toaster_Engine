#pragma once
#include "Module.h"
#include "Globals.h"
#include "../External/ImGui/imgui.h"

#include "GameObject.h"

#include "FileTree.cpp"

#define MAX_LOGS_SIZE 100

class GameObject;
class FileTree;

struct HierarchyNaming {
	std::string dd_string;
	int dd_counters = 1;
};

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

public:
	void Space();

public:
	// Inspector
	std::vector<GameObject*> gameObjects;

	uint goID = 0;

	GameObject* selectedGameObj;
	GameObject* root;
	GameObject* draggingGO = nullptr;

	uint AddGameObject(GameObject* GameObj);
	void SetSelectedGameObject(GameObject* GameObj);
	GameObject* GetSelectedGameObject() { return selectedGameObj; }

	void PrepareDrawGameObject(GameObject* gameObj, bool hasCh);
	void DrawGameObject(GameObject* gameObj, int iteration);

	GameObject* printedGameObj = nullptr;

	// Assets
	void AssetTree(FileTree* node);

	FileTree* fileTree;
	FileTree* currentNode = nullptr;
	std::vector<std::string> allFiles;

	std::string dd_file_name = "";
	int ddCooldown = 9999;
	std::string message = "";

	std::vector<HierarchyNaming> Hnaming;

	uint texture = NULL;
	uint actualTexture;
	uint checkers_texture;

private:
	std::string ddname = "New GameObject";

public:
	void AddLogMsg(const char* msg);
};