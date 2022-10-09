#include "Application.h"
#include "ModuleEditor.h"

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled){}

ModuleEditor::~ModuleEditor() {}

bool ModuleEditor::Start() {
	LOG("Starting Editor");
	return true;
}

update_status ModuleEditor::PreUpdate(float dt) {

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update(float dt) {

	

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate(float dt) {


	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp() {

	return true;
}

void ModuleEditor::Draw(){

	if (ImGui::BeginMenu("Help")) {
		if (ImGui::MenuItem("About Demo")) {
			ImGui::ShowAboutWindow();
		}

		if (ImGui::MenuItem("Debug Log Demo")) {
			ImGui::ShowDebugLogWindow();
		}

		if (ImGui::MenuItem("Engine Documentation")) {
			OpenURL("https://github.com/Dani-24/Toaster_Engine");
		}

		ImGui::EndMenu();
	}

	//ImGui::EndMainMenuBar();

}