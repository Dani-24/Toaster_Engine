#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"

#include "../External/Mmgr/mmgr.h"
#include "OpenGL.h"

#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled){
	logs.reserve(MAX_LOGS_SIZE);
	fpslog.reserve(MAX_LOGS_SIZE);
	mslog.reserve(MAX_LOGS_SIZE);
	memlog.reserve(MAX_LOGS_SIZE);
}

ModuleEditor::~ModuleEditor() {}

bool ModuleEditor::Start() {
	LOG("Starting Editor");
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
			if (ImGui::MenuItem("New Toast", "WIP")) {

			}
			if (ImGui::MenuItem("Open Toast", "WIP")) {

			}
			if (ImGui::MenuItem("Save your Toast", "WIP")) {

			}
			if (ImGui::MenuItem("Save and Close the fridge", "WIP")) {

			}
			if (ImGui::MenuItem("Suicide Butter", "Ctrl+Alt+F4", &closeOpenClose)) {

			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "WIP")) {

			}
			if (ImGui::MenuItem("Redo", "WIP")) {

			}
			if (ImGui::MenuItem("AMOng US?", "SUS")) {
				OpenURL("https://youtu.be/fMcWA544-5A");
				OpenURL("https://static.wikia.nocookie.net/shitpost/images/2/2a/Amogus.png/revision/latest?cb=20210717210340&path-prefix=es");
			}
			if (ImGui::MenuItem("Toaster Mode", "Just work at toaster speed", &toasterMode)) {}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Camera")) {
			if (app->camera->psychoControls) {
				if (ImGui::MenuItem("Disable Phycopath Camera Controls", "Disable ALT inputs"))
				{
					app->camera->psychoControls = false;
				}
			}
			else {
				if (ImGui::MenuItem("Enable Phycopath Camera Controls", "Enable ALT inputs"))
				{
					app->camera->psychoControls = true;
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window")) {

			// SHOW EDITOR
			if (showingEditor)
			{
				if (ImGui::MenuItem("Hide All Editor Windows"))
				{
					showingEditor = !showingEditor;

					showConsoleMenu = showHierarchy = showInspector = showGameEditorWindow = showAssetManager = false;
				}
			}
			else {
				if (ImGui::MenuItem("Show All Editor Windows"))
				{
					showingEditor = !showingEditor;

					showConsoleMenu = showHierarchy = showInspector = showGameEditorWindow = showAssetManager = true;

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
		//ImGui::Image();
		ImGui::TextWrapped("WIP Window");

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
		
		ImGui::TextWrapped("WIP");
		
		ImGui::End();
	}
}

void ModuleEditor::ShowAssetExplorer(bool* open) {
	if (!ImGui::Begin("Asset Explorer", open)) {
		ImGui::End();
	}
	else {

		ImGui::TextWrapped("WIP");

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

		// Do smtg
		ImGui::TextWrapped("WIP");

		ImGui::End();
	}
}

void ModuleEditor::ShowHierarchyMenu(bool* open) {
	if (!ImGui::Begin("Scene Hierarchy", open)) {
		ImGui::End();
	}
	else {
		// Do smtg
		ImGui::TextWrapped("WIP");

		ImGui::End();
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
		ImGui::TextWrapped("Assimp: ?");

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