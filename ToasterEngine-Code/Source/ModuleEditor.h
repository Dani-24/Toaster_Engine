#pragma once
#include "Module.h"
#include "Globals.h"
#include "../External/ImGui/imgui.h"

#include "GameObject.h"

#include "FileTree.cpp"

#include "ModuleImporter.h"

#define MAX_LOGS_SIZE 100

class GameObject;
class FileTree;

struct HierarchyNaming {
	std::string dd_string;
	int dd_counters = 1;
};

enum class DragDropFile {
	MESH,
	TEXTURE,
	OTHER
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

	int SomeRandomIntValue() { return RandomIntValue(); };

private:

	float cooldown = 100, maxCooldown = 100, minCooldown = 0;
	bool exit = false;
	bool showingEditor = true;

	// Closes the Program
	void AreYouSureAboutThat(bool *open);

	// Gameobject Inspector
	void ShowInspectorMenu(bool* open);

	// Show all your gameobjects at the Scene
	void ShowHierarchyMenu(bool* open);

	// Engine Console Log
	void ShowConsoleMenu(bool* open);

	// Info About the toaster
	void ShowAboutMenu(bool* open);

	// Game & Editor Camera views
	void ShowGameEditorWindow(bool* open);

	// Engine Configuration
	void ShowConfiguration(bool* open);

	// Show Engine/Game Assets
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

	GameObject* selectedGameObj = nullptr;
	GameObject* root = nullptr;
	GameObject* draggingGO = nullptr;

	// Adds a GameObject to the editor GO list (Called from GameObject.cpp)
	uint AddGameObject(GameObject* GameObj);

	// Set/get the Selected GameObject
	void SetSelectedGameObject(GameObject* GameObj);
	GameObject* GetSelectedGameObject() { return selectedGameObj; }

	// Draw GameObjects on Hierarchy
	void PrepareDrawGameObject(GameObject* gameObj, bool hasCh);
	void DrawGameObject(GameObject* gameObj, int iteration);

	GameObject* printedGameObj = nullptr;

	// Assets as a tree
	void AssetTree(FileTree* node);

	FileTree* fileTree = nullptr;
	FileTree* currentNode = nullptr;
	std::vector<std::string> allFiles;

	std::string dd_file_name = "";
	int ddCooldown = 9999;
	std::string message = "";
	ResourceType dd_file_type;
	std::vector<HierarchyNaming> Hnaming;

	bool showAllAABB = false;

	vec2 editorPos = vec2(0, 0);
	vec2 editorSize = vec2(0, 0);

private:
	std::string ddname = "New GameObject";

	int assetsReload = 100;

public:
	// Log a const char*
	void AddLogMsg(const char* msg);

	bool playing = false;
	bool paused = false;

	std::vector<C_Mesh*> renderQueue;
	std::multimap<float, C_Mesh*> renderQueueMap;
};