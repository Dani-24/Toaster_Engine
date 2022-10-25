#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"

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

	return true;
}

void ModuleEditor::Draw(){

	static bool closeOpenClose = false;
	static bool showDemoWindow = false;

	if (closeOpenClose) AreYouSureAboutThat(&closeOpenClose);
	if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New", "Ctrl+N")) {

			}

			if (ImGui::MenuItem("Open", "Ctrl+O")) {

			}

			if (ImGui::MenuItem("Save", "Ctrl+S")) {

			}

			if (ImGui::MenuItem("Save and Close", "Ctrl+Alt+S")) {

			}

			if (ImGui::MenuItem("Suicide", "Ctrl+Alt+F4", &closeOpenClose)) {

			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {
			ShowEditMenu();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help")) {
			if (ImGui::MenuItem("Show Demo Window",NULL, &showDemoWindow)) {

			}

			if (ImGui::MenuItem("Debug Log"))
			{
				
			}

			if (ImGui::MenuItem("Engine Documentation"))
			{
				OpenURL("https://github.com/Dani-24/Toaster_Engine");
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

}

void ModuleEditor::AreYouSureAboutThat(bool *open) {

	ImGui::SetNextWindowSize(ImVec2(500, 400), 4);

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

void ModuleEditor::ShowEditMenu() {
	if (ImGui::MenuItem("Undo", "Ctrl+Z")) {

	}
	if (ImGui::MenuItem("Redo", "Ctrl+Alt+Z")) {

	}
	if (ImGui::MenuItem("AMOng US?", "Ctrl+SUS")) {

	}
	if (ImGui::MenuItem("Show Grid", "Or Hide it")) {
		app->scene->axis = !app->scene->axis;
	}
}