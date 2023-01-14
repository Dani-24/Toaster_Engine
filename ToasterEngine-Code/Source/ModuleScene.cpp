#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"

#include "../External/MathGeoLib/include/MathGeoLib.h"

#include "OpenGL.h"

#include "ModuleMesh3D.h"
#include "ModuleTexture.h"

#include "GameObject.h"
#include "C_Transform.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start()
{
	LOG("TOASTER: Preparing Default Scene");
	bool ret = true;

	GameObject* skybox = new GameObject("skybox", nullptr);
	skybox->AddComponent(Component::TYPE::MATERIAL, "Assets/default_Meshes/skybox.png");
	skybox->AddComponent(Component::TYPE::MESH, "Assets/default_Meshes/skybox.fbx");
	skybox->transform->localScale.Set(200, 200, 200);
	skybox->transform->eulerRotation.Set(90, 0, 0);

	GameObject* openGameObject = new GameObject("Demo Street Environment", app->editor->root);
	openGameObject->AddComponent(Component::TYPE::MATERIAL, "Assets/street_text1.png");
	openGameObject->AddComponent(Component::TYPE::MATERIAL, "Assets/street_text2.png");
	openGameObject->AddComponent(Component::TYPE::MESH, "Assets/street.fbx");

	return ret;
}

update_status ModuleScene::PreUpdate(float dt) {


	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt) {
	if (axis) {
		PlanePrimitive p(0, 1, 0, 0);
		p.axis = true;
		p.Render();
	}

	if(app->renderer3D->renderOnThisCamera != app->camera->editorCamera){
		DrawLines();
	}

	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	LOG("Unloading scene");

	return true;
}

void ModuleScene::AddLines(float3 line, Color c) {
	f3line fline;

	fline.line = line;
	fline.color = c;

	lines.push_back(fline);
}

void ModuleScene::DrawLines() {

	for (int i = 0; i < lines.size(); i++) {
		LinePrimitive drawline;
		drawline.origin = vec3(lines[i].line.x, lines[i].line.y, lines[i].line.z);
		drawline.destination = vec3(lines[i+1].line.x, lines[i+1].line.y, lines[i+1].line.z);
		drawline.color = lines[i].color;
		i++; // +2

		drawline.Render();
	}

	lines.clear();
}