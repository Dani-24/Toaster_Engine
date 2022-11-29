﻿#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"

#include "../External/Mmgr/mmgr.h"
#include "OpenGL.h"

#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"

#include "Primitive.h"

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled){
	logs.reserve(MAX_LOGS_SIZE);
	fpslog.reserve(MAX_LOGS_SIZE);
	mslog.reserve(MAX_LOGS_SIZE);
	memlog.reserve(MAX_LOGS_SIZE);
}

ModuleEditor::~ModuleEditor() {
}

bool ModuleEditor::Start() {
	LOG("TOASTER: Loading Editor");

	root = new GameObject("Scene", nullptr);

	fileTree = currentNode = ModuleImporter::GetFileTree("Assets");
	allFiles.clear();
	allFiles = ModuleImporter::GetAllFiles("Assets");

	checkers_texture = app->textures->ImportTexture("Assets/Checkers.png");

	return true;
}

update_status ModuleEditor::PreUpdate(float dt) 
{
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update(float dt) {

	// Load FPS/MS/MEM logs

	if (showingEditor) {
		// FPS
		if (fpslog.size() <= MAX_LOGS_SIZE) {
			fpslog.push_back(1.f/dt);
		}
		else {
			fpslog.erase(fpslog.begin());
			fpslog.push_back(1.f / dt);
		}

		// MS
		if (mslog.size() <= MAX_LOGS_SIZE) {
			mslog.push_back(dt);
		}
		else {
			mslog.erase(mslog.begin());
			mslog.push_back(dt);
		}
	}

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate(float dt) {

	// Delete GameObjects
	for (int i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i]->pendindToDelete) {
			gameObjects[i]->DeleteThisGameObject();
			gameObjects.erase(gameObjects.begin() + i);
		}
	}

	// New Scene
	if (!root->childs.empty() && newScene == true) {
		for (int i = 0; i < root->childs.size(); i++) {
			root->childs[i]->DeleteThisGameObject();
		}
		if (root->childs.empty()) {
			newScene = false;
			goID = 1;
			Hnaming.clear();
		}
	}

	for (int i = 0; i < gameObjects.size(); i++) {
		// Render GO
		gameObjects[i]->RenderMesh();
	}

	if (!exit) {
		return UPDATE_CONTINUE;
	}
	else {
		return UPDATE_STOP;
	}
}

bool ModuleEditor::CleanUp() {
	for (int i = 0; i < logs.size(); i++) {
		delete logs[i].data();
		logs[i] = nullptr;
	}
	logs.clear();
	fpslog.clear();
	mslog.clear();
	memlog.clear();
	gameObjects.clear();

	RELEASE(fileTree);

	return true;
}

void ModuleEditor::Draw(){

	static bool closeOpenClose = false;
	static bool showDemoWindow = false;
	static bool showAboutMenu = false;
	static bool showConsoleMenu = true;
	static bool showHierarchy = true;
	static bool showInspector = true;
	static bool showGameEditorWindow = false;
	static bool showConfiguration = false;
	static bool toasterMode = false;
	static bool showAssetManager = true;
	static bool showAssetExplorer = true;
	
	if (toasterMode)			{ app->maxFps = 10; } else { app->maxFps = 60; }
	if (closeOpenClose)			AreYouSureAboutThat(&closeOpenClose);
	if (showDemoWindow)			ImGui::ShowDemoWindow(&showDemoWindow);
	if (showConsoleMenu)		ShowConsoleMenu(&showConsoleMenu);
	if (showAboutMenu)			ShowAboutMenu(&showAboutMenu);
	if (showHierarchy)			ShowHierarchyMenu(&showHierarchy);
	if (showInspector)			ShowInspectorMenu(&showInspector);
	if (showGameEditorWindow)	ShowGameEditorWindow(&showGameEditorWindow);
	if (showConfiguration)		ShowConfiguration(&showConfiguration);
	if (showAssetManager)		ShowAssetManager(&showAssetManager);
	if (showAssetExplorer)		ShowAssetExplorer(&showAssetExplorer);

	// File/Edit/window main engine bar
	if (ImGui::BeginMainMenuBar()) {

		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New Toast", "Ctrl+N")) {

				newScene = true;

				root->name = "New Scene";
			}
			if (ImGui::MenuItem("Open Toast", "WIP")) {

			}
			if (ImGui::MenuItem("Save your Toast", "WIP")) {

			}
			if (ImGui::MenuItem("Save and Close the fridge", "WIP")) {

			}
			if (ImGui::MenuItem("Close Butter", "Ctrl+Alt+F4", &closeOpenClose)) {

			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "WIP")) {

			}
			if (ImGui::MenuItem("Redo", "WIP")) {

			}
			if (ImGui::MenuItem("Toaster Mode", "Just work at toaster speed", &toasterMode)) {}
			if (ImGui::BeginMenu("Create 3D Mesh")) {
				if(ImGui::MenuItem("Cube")) {

					GameObject* cube = new GameObject("Cube", root);
					cube->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/cube.fbx"));
				}
				if (ImGui::MenuItem("Sphere")) {

					GameObject* sphere = new GameObject("Sphere", root);
					sphere->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/sphere.fbx"));
				}
				if (ImGui::MenuItem("Cylinder")) {

					GameObject* cylinder = new GameObject("Cylinder", root);
					cylinder->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/cylinder.fbx"));
				}
				if (ImGui::MenuItem("Cone")) {

					GameObject* cone = new GameObject("Cone", root);
					cone->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/cone.fbx"));
				}
				if (ImGui::MenuItem("Plane")) {

					GameObject* plane = new GameObject("Plane", root);
					plane->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/plane.fbx"));
				}
				if (ImGui::MenuItem("Demo: Baker House")) {

					GameObject* house = new GameObject("Baker House", root);
					house->AddMesh(app->mesh3d->LoadFile("Assets/BakerHouse.fbx"));
					house->AddTexture(app->textures->ImportTexture("Assets/Baker_house.png"));
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Camera")) {
			
			ImGui::Text("Add new camera???");

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window")) {

			// SHOW EDITOR
			if (showingEditor)
			{
				if (ImGui::MenuItem("Hide All Editor Windows"))
				{
					showingEditor = !showingEditor;

					showConsoleMenu = showHierarchy = showInspector = showGameEditorWindow = showAssetManager = showAssetExplorer = false;
				}
			}
			else {
				if (ImGui::MenuItem("Show All Editor Windows"))
				{
					showingEditor = !showingEditor;

					showConsoleMenu = showHierarchy = showInspector = showGameEditorWindow = showAssetManager = showAssetExplorer = true;

				}
			}

			// GRID AXIS
			if (app->scene->axis) {
				if (ImGui::MenuItem("Hide Grid")) {
					app->scene->axis = !app->scene->axis;
				}
			}
			else {
				if (ImGui::MenuItem("Show Grid")) {
					app->scene->axis = !app->scene->axis;
				}
			}

			// GAME EDITOR WINDOW
			if (ImGui::MenuItem("Game Editor Window", NULL, &showGameEditorWindow)) {};

			// ASSET MANAGER
			if (ImGui::MenuItem("Asset Manager", NULL, &showAssetManager)) {};

			// ASSET EXPLORER
			if (ImGui::MenuItem("Asset Explorer", NULL, &showAssetExplorer)) {};

			// DEBUG CONSOLE
			if (ImGui::MenuItem("Debug Console", NULL, &showConsoleMenu)) {};

			// HIERARCHY DISPLAY
			if (ImGui::MenuItem("Hierarchy", NULL, &showHierarchy)) {}

			// INSPECTOR DISPLAY
			if (ImGui::MenuItem("Inspector", NULL, &showInspector)) {}

			// CONFIGURATION
			if (ImGui::MenuItem("Configuration", NULL, &showConfiguration)) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help")) {

			if (ImGui::MenuItem("About", NULL, &showAboutMenu)) {}
			if (ImGui::MenuItem("Show Demo Window",NULL, &showDemoWindow)) {}
			if (ImGui::MenuItem("Engine Documentation"))
			{
				OpenURL("https://github.com/Dani-24/Toaster_Engine");
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void ModuleEditor::ShowGameEditorWindow(bool* open) {
	if (!ImGui::Begin("Game Editor", open)) {
		ImGui::End();
	}
	else {

		// RENDER CAMERA HERE

		//with out the size, rescalet okey but crash
		//ImGui::BeginChild("", ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT));

		//ImVec2 sizeWindScn = ImGui::GetWindowSize();

		////ImGui::Image((ImTextureID)app->renderer3D->camBuff, sizeWindScn, ImVec2(0, 1), ImVec2(1, 0));

		//ImGui::EndChild();		

		ImGui::End();
	}
}

void ModuleEditor::AreYouSureAboutThat(bool *open) {

	int sizeX = 300;
	int sizeY = 60;
	ImGui::SetNextWindowSize(ImVec2(sizeX, sizeY));
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 2 - sizeX / 2, SCREEN_HEIGHT / 2 - sizeY / 2));

	if (!ImGui::Begin("Closing this toaster in :", open)) {
		ImGui::End();
	}
	else{

		if (cooldown <= minCooldown) {
			exit = true;
		}
		else {
			cooldown -= 2;
		}

		char progressText[32];
		sprintf(progressText, "%.0f/%.0f", cooldown, maxCooldown);

		ImGui::ProgressBar(cooldown / maxCooldown, ImVec2(-1, 0), progressText);

		ImGui::End();
	}

}

void ModuleEditor::ShowConsoleMenu(bool *open) {

	if (!ImGui::Begin("Console LOG", open)) {
		ImGui::End();
	}
	else {

		for (int i = logs.size() -1; i > 0; i--) {
			ImGui::TextWrapped(logs[i].c_str());
		}

		ImGui::End();
	}
}

void ModuleEditor::ShowAssetManager(bool* open) {
	if (!ImGui::Begin("Assets", open)) {
		ImGui::End();
	}
	else {
		
		uint rows = 0;
		uint dir_size = currentNode->directories.size();
		for (uint i = 0; i < dir_size; i++)
		{
			std::string s;
			if (currentNode->directories.at(i)->files.empty()) {
				s = "Open Folder\n";
			}
			else {
				s = "Closed Folder\n";
			}
			s += currentNode->directories[i]->name;

			ImGui::SameLine();
		}
		for (size_t i = dir_size; i < currentNode->files.size() + dir_size; i++)
		{
			std::string s = "File \n";
			s += currentNode->files[i - dir_size];

			ImGui::Button(s.c_str(), ImVec2(120, 120));

			if (ddCooldown > 30 && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover | ImGuiDragDropFlags_SourceNoPreviewTooltip))
			{
				std::string file_path = currentNode->path + currentNode->files[i - dir_size];
				ImGui::SetDragDropPayload(file_path.c_str(), &i, sizeof(std::string));
				dd_file_name = file_path;

				ddname = currentNode->files[i - dir_size];
				ddname = ddname.substr(0, ddname.find_last_of("."));

				for (int i = 0; i < Hnaming.size(); i++) {
					if (Hnaming[i].dd_string == ddname) {
						char a = '0' + Hnaming[i].dd_counters;
						ddname.push_back(' '); 
						ddname.push_back('(');
						ddname.push_back(' ');
						ddname.push_back(a);
						ddname.push_back(' '); 
						ddname.push_back(')');
						Hnaming[i].dd_counters++;
						break;
					}
					else {
						HierarchyNaming d; d.dd_string = ddname;
						Hnaming.push_back(d);
					}
				}
				if (Hnaming.empty()) {
					HierarchyNaming d; d.dd_string = ddname;
					Hnaming.push_back(d);
				}

				ImGui::EndDragDropSource();
			}

			ImGui::SameLine();
		}
		
		ImGui::End();
	}
	if (dd_file_name != "") {
		GameObject* ddFile = new GameObject(ddname, root);
		ddFile->AddMesh(app->mesh3d->LoadFile(dd_file_name));
		ddFile->AddTexture(app->textures->ImportTexture(dd_file_name));
		dd_file_name = "";
		ddCooldown = 0;
	}
	ddCooldown++;
}

void ModuleEditor::ShowAssetExplorer(bool* open) {
	if (!ImGui::Begin("Asset Explorer", open)) {
		ImGui::End();
	}
	else {

		ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
		
		AssetTree(fileTree);

		ImGui::End();
	}
}

void ModuleEditor::AddLogMsg(const char* msg) {

	if (logs.size() <= MAX_LOGS_SIZE) {
		logs.push_back(msg);
	}
	else {
		logs.erase(logs.begin());
		logs.push_back(msg);
	}
}

void ModuleEditor::ShowInspectorMenu(bool* open) {
	if (!ImGui::Begin("Inspector", open)) {
		ImGui::End();
	}
	else {

		if (selectedGameObj != nullptr) {
			
			ImGui::InputText(" ", (char*)selectedGameObj->name.c_str(), 50);

			ImGui::SameLine();

			ImGui::TextWrapped("ID: %d", selectedGameObj->GetID());

			Space();

			if (selectedGameObj != root) { // Don't allow to delete the root
				
				selectedGameObj->OnEditor();

				Space();

				// Delete Game Object
				if (selectedGameObj->GetParent() != nullptr) {
					ImGui::Selectable("Delete this Game Object ", &selectedGameObj->pendindToDelete);
				}
			}
		}
		else {

			ImGui::TextWrapped("There is no GameObject selected");
		}

		ImGui::End();
	}
}

void ModuleEditor::ShowHierarchyMenu(bool* open) {
	if (!ImGui::Begin("Hierarchy", open)) {
		ImGui::End();
	}
	else {
		if (!gameObjects.empty()) {
			PrepareDrawGameObject(gameObjects[0], false);
		}
		ImGui::End();
	}
}

void ModuleEditor::PrepareDrawGameObject(GameObject* gameObj, bool hasCh) {
	if (!hasCh) {
		DrawGameObject(gameObj, 0);
	}
	else {
		for (uint i = 0; i < gameObj->childs.size(); i++)
		{
			DrawGameObject(gameObj->childs[i], i);
		}
	}
}

void ModuleEditor::DrawGameObject(GameObject* gameObj, int iteration) {

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

	if (gameObj == GetSelectedGameObject()) {
		node_flags |= ImGuiTreeNodeFlags_Selected;
	}

	bool node_open;

	if (gameObj->childs.empty()) // WITHOUT CHILDS
	{
		ImGui::AlignTextToFramePadding();
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

		ImGui::TreeNodeEx((void*)(intptr_t)iteration, node_flags, gameObj->GetName().c_str(), iteration);

		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
		{
			if (ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left)) SetSelectedGameObject(gameObj);
		}
		node_open = false;
	}
	else // WITH CHILDS
	{
		ImGui::AlignTextToFramePadding();
		node_flags |= ImGuiTreeNodeFlags_AllowItemOverlap;
		node_open = ImGui::TreeNodeEx((void*)(intptr_t)iteration, node_flags, gameObj->GetName().c_str(), iteration);

		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
		{
			if (ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left)) SetSelectedGameObject(gameObj);
		}
	}

	// Drag Drop
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("GameObject", gameObj, sizeof(GameObject*));

		draggingGO = gameObj;
		ImGui::Text("Make Toasty Child?");

		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
		{
			if (gameObj->GetParent() != draggingGO) { // Avoid making a parent of a child of a parent of a child of a parent of a child of a parent of a child of a parent
				draggingGO->GetParent()->DeleteChild(draggingGO);
				gameObj->AddChild(draggingGO);
				draggingGO = nullptr;
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (node_open)
	{
		if (!gameObj->childs.empty()) {
			PrepareDrawGameObject(gameObj, true);
		}
		ImGui::TreePop();
	}
}

void ModuleEditor::ShowAboutMenu(bool* open) {
	if (!ImGui::Begin("About toasts", open)) {
		ImGui::End();
	}
	else {

		// INFO
		ImGui::TextWrapped("TOASTER Engine v0.0.1"); // A VERSION GETTER THERE WOULD BE COOL
		
		Space();

		ImGui::TextWrapped("Amazingly developed by :");
		if (ImGui::MenuItem("Dani Toledo || Dani24"))
		{
			ShellExecute(0, 0, "https://github.com/Dani-24", 0, 0, SW_SHOW);
		}

		ImGui::TextWrapped("Help me pls");

		Space();

		// SOFTWARE VERSIONS
		char SDLVersion[25];
		SDL_version version;
		SDL_GetVersion(&version);
		sprintf_s(SDLVersion, 25, "%i.%i.%i", version.major, version.minor, version.patch);

		ImGui::TextWrapped("3rd Party Libraries used in this toaster:"); ImGui::NewLine();
		ImGui::TextWrapped("SDL Version: %s", SDLVersion); ImGui::NewLine();
		ImGui::TextWrapped("OpenGL Version: %s", glGetString(GL_VERSION)); ImGui::NewLine();
		ImGui::TextWrapped("Glew Version: %s", glewGetString(GLEW_VERSION)); ImGui::NewLine();
		ImGui::TextWrapped("MathGeoLib Version: 1.5"); ImGui::NewLine();
		ImGui::TextWrapped("ImGui Version: 1.89"); ImGui::NewLine();
		ImGui::TextWrapped("Mmgr: Fluid Studios"); ImGui::NewLine();
		ImGui::TextWrapped("GPUDetector : Intel Corporation"); ImGui::NewLine();
		ImGui::TextWrapped("Parson: 1.1.0"); ImGui::NewLine();
		ImGui::TextWrapped("Assimp: 5.2.5"); ImGui::NewLine();
		ImGui::TextWrapped("DevIL: 1.8.0"); ImGui::NewLine();
		ImGui::TextWrapped("PhysFS: 3.0.2");

		Space();

		// LICENSE
		ImGui::TextWrapped("MIT License");
		ImGui::TextWrapped("Copyright(c) 2022 Dani - 24");
		ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copy of this softwareand associated documentation files(the `Software`), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions : ");
		ImGui::TextWrapped("The above copyright noticeand this permission notice shall be included in all copies or substantial portions of the Software.");
		ImGui::TextWrapped("THE SOFTWARE IS PROVIDED `AS IS`, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");

		ImGui::End();
	}
}

void ModuleEditor::ShowConfiguration(bool* open) {
	if (!ImGui::Begin("Configuration", open)) {

		// Clean logs bc who cares
		fpslog.clear();
		mslog.clear();
		memlog.clear();

		ImGui::End();
	}
	else {
		if (ImGui::CollapsingHeader("Toaster Application", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputText("This thing name", "Toaster Engine", 13);

			ImGui::InputText("Creator", "Dani Toledo", 12);

			Space();

			// Graphs
			if (ImGui::SliderInt("Maximum FPS", &app->maxFps, 1, 60))
			{
				// Should disable Vsync there is this engine had Vsync XD
			}

			// FPS
			char IDKWhatThisCharDoesButItsRequired[50];
			sprintf_s(IDKWhatThisCharDoesButItsRequired, 50, "This Toaster Framerate %.1f", fpslog[fpslog.size() - 1]);
			ImGui::PlotHistogram("##framerate", &fpslog[0], fpslog.size(), 0, IDKWhatThisCharDoesButItsRequired, 0.0f, app->maxFps * 1.2f, ImVec2(300, 100));
			sprintf_s(IDKWhatThisCharDoesButItsRequired, 50, "To the one user who cares about milliseconds %0.1f", mslog[mslog.size() - 1]);
			ImGui::PlotHistogram("##milliseconds", &mslog[0], mslog.size(), 0, IDKWhatThisCharDoesButItsRequired, 0.0f, 1.f/app->maxFps * 1.5f, ImVec2(300, 100));
			
			Space();

			// MEMORY
			sMStats stats = m_getMemoryStatistics();
			if (memlog.size() <= MAX_LOGS_SIZE) {
				memlog.push_back(stats.totalReportedMemory);
			}
			else {
				memlog.erase(memlog.begin());
				memlog.push_back(stats.totalReportedMemory);
			}

			ImGui::PlotHistogram("##memoryXD", &memlog[0], memlog.size(), 0, "Toasty Memory Consumption", 0.0f, (float)stats.peakReportedMemory * 1.2f, ImVec2(300, 100));

			ImGui::NewLine();

			ImGui::TextWrapped("TOP 10 Bunch of Useless info here for memory fans :");

			ImGui::NewLine();

			ImGui::TextWrapped("Total Reported Mem: %d", stats.totalReportedMemory);
			ImGui::TextWrapped("Total Actual Mem: %d", stats.totalActualMemory);
			ImGui::TextWrapped("Peak Reported Mem: %d", stats.peakReportedMemory);
			ImGui::TextWrapped("Peak Actual Mem: %d", stats.peakActualMemory);
			ImGui::TextWrapped("Accumulated Reported Mem: %d", stats.accumulatedReportedMemory);
			ImGui::TextWrapped("Accumulated Actual Mem: %d", stats.accumulatedActualMemory);
			ImGui::TextWrapped("Accumulated Alloc Unit Count: %d", stats.accumulatedAllocUnitCount);
			ImGui::TextWrapped("Total Alloc Unit Count: %d", stats.totalAllocUnitCount);
			ImGui::TextWrapped("Peak Alloc Unit Count: %d", stats.peakAllocUnitCount);
		}
		if (ImGui::CollapsingHeader("Window configuration")) {
			app->window->GUIinfo();
		}
		if (ImGui::CollapsingHeader("Toast Rendering OpenGL Options")) {
			app->renderer3D->GUIglInfo();
		}
		if (ImGui::CollapsingHeader("Toaster Input")) {
			app->input->GUIinfo();
		}
		if (ImGui::CollapsingHeader("Your Toaster Advanced Hardware")) {
			app->renderer3D->GUIhardwareInfo();
		}

		ImGui::End();
	}
}

void ModuleEditor::Space() {
	ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();
}

uint ModuleEditor::AddGameObject(GameObject* GameObj) {

	gameObjects.push_back(GameObj);

	goID++;

	return goID - 1;
}

void ModuleEditor::SetSelectedGameObject(GameObject* GameObj) {
	selectedGameObj = GameObj;
}

void ModuleEditor::AssetTree(FileTree* node) {
	bool opened = ImGui::TreeNodeEx(node->name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) currentNode = node;
	if (opened)
	{
		for (size_t i = 0; i < node->directories.size(); i++)
		{
			AssetTree(node->directories[i]);
		}
		for (size_t i = 0; i < node->files.size(); i++)
		{
			ImGui::Text(node->files[i].c_str());
		}
		ImGui::TreePop();
	}
}