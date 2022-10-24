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

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			ShowFileMenu();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {
			ShowEditMenu();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help")) {
			SendHelpPls();
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	static bool closeOpenClose = true;

	if(showCloseMenu) AreYouSureAboutThat(closeOpenClose);
}

void ModuleEditor::ShowFileMenu() {
	if (ImGui::MenuItem("New", "Ctrl+N")) {
		
	}

	if (ImGui::MenuItem("Open", "Ctrl+O")) {

	}

	if (ImGui::MenuItem("Save", "Ctrl+S")) {

	}

	if (ImGui::MenuItem("Save and Close", "Ctrl+Alt+F4")) {

	}

	if (ImGui::MenuItem("Suicide", "Ctrl+Alt+F4")) {
		showCloseMenu = true;
	}
}

void ModuleEditor::AreYouSureAboutThat(bool open) {

	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("This shit will close in :", &open)) {
		ImGui::End();
	}
	else{

		if (cooldown >= maxCooldown) {
		}
		else {
			cooldown += 1;
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
}

void ModuleEditor::SendHelpPls() {

	if (ImGui::MenuItem("About Demo")) {
		ImGui::ShowAboutWindow();
	}

	if (ImGui::MenuItem("Debug Log Demo"))
	{
		ImGui::ShowDebugLogWindow();
	}
	
	if (ImGui::MenuItem("Engine Documentation"))
	{
		OpenURL("https://github.com/Dani-24/Toaster_Engine");
	}
}