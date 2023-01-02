#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"

#include "../External/MathGeoLib/include/MathGeoLib.h"

#include "OpenGL.h"

#include "ModuleMesh3D.h"
#include "ModuleTexture.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start()
{
	LOG("TOASTER: Preparing Default Scene");
	bool ret = true;

	GameObject* openGameObject = new GameObject("Demo Street Environment", app->editor->root);
	openGameObject->AddTexture(app->textures->LoadTexture("Assets/street_text1.png"));
	openGameObject->AddTexture(app->textures->LoadTexture("Assets/street_text2.png"));
	openGameObject->AddMesh(app->mesh3d->LoadFile("Assets/street.fbx", openGameObject));

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