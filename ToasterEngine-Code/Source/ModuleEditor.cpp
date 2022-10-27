#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled){}

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

	return true;
}

void ModuleEditor::Draw(){

	static bool closeOpenClose = false;
	static bool showDemoWindow = false;
	static bool showAboutMenu = false;
	static bool showConsoleMenu = true;
	static bool showHierarchy = true;
	static bool showInspector = true;
	static bool showGameEditorWindow = true;

	if (closeOpenClose)		AreYouSureAboutThat(&closeOpenClose);
	if (showDemoWindow)		ImGui::ShowDemoWindow(&showDemoWindow);
	if (showConsoleMenu)	ShowConsoleMenu(&showConsoleMenu);
	if (showAboutMenu)		ShowAboutMenu(&showAboutMenu);
	if (showHierarchy)		ShowHierarchyMenu(&showHierarchy);
	if (showInspector)		ShowInspectorMenu(&showInspector);
	if (showGameEditorWindow)ShowGameEditorWindow(&showGameEditorWindow);

	if (ImGui::BeginMainMenuBar()) {

		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New", "WIP")) {

			}
			if (ImGui::MenuItem("Open", "WIP")) {

			}
			if (ImGui::MenuItem("Save", "WIP")) {

			}
			if (ImGui::MenuItem("Save and Close", "WIP")) {

			}
			if (ImGui::MenuItem("Suicide", "Ctrl+Alt+F4", &closeOpenClose)) {

			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "WIP")) {

			}
			if (ImGui::MenuItem("Redo", "WIP")) {

			}
			if (ImGui::MenuItem("AMOng US?", "WIP")) {

			}
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
				if (ImGui::MenuItem("Hide editor windows", "WIP"))
				{
					showingEditor = !showingEditor;
				}
			}
			else {
				if (ImGui::MenuItem("Show editor windows", "WIP"))
				{
					showingEditor = !showingEditor;
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

			// DEBUG CONSOLE
			if (ImGui::MenuItem("Debug Console", NULL, &showConsoleMenu)) {};

			// HIERARCHY DISPLAY
			if (ImGui::MenuItem("Hierarchy", NULL, &showHierarchy)) {}

			// INSPECTOR DISPLAY
			if (ImGui::MenuItem("Inspector", NULL, &showInspector)) {}

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

	// EDITOR WINDOW
	int fixY = 18;
	ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT - fixY));
	ImGui::SetNextWindowPos(ImVec2(0, fixY));

	ImGui::Begin("Editor");
	ImGui::End();

}

void ModuleEditor::ShowGameEditorWindow(bool* open) {
	if (!ImGui::Begin("Game Editor", open)) {
		ImGui::End();
	}
	else {

		// RENDER CAMERA HERE
		//ImGui::Image();

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

void ModuleEditor::AddLogMsg(const char* msg) {
	logs.push_back(msg);
}

void ModuleEditor::ShowConfigMenu(bool* open) {
	if (!ImGui::Begin("Configuration", open)) {
		ImGui::End();
	}
	else {
		// Do smtg
		ImGui::TextWrapped("WIP");

		ImGui::End();
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
	if (!ImGui::Begin("About", open)) {
		ImGui::End();
	}
	else {
		// Do smtg
		ImGui::TextWrapped("WIP");

		ImGui::End();
	}
}