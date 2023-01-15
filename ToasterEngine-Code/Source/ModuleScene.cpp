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

	GameObject* skybox = new GameObject("skybox", nullptr);
	skybox->AddTexture(app->textures->LoadTexture("Assets/default_Meshes/skybox.png"));
	skybox->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/skybox.fbx", skybox));
	skybox->SetScale(vec3(200, 200, 200));
	skybox->SetRot(vec3(90, 0, 0));

	GameObject* openGameObject = new GameObject("Demo Street Environment", app->editor->root);
	openGameObject->AddTexture(app->textures->LoadTexture("Assets/street_text1.png"));
	openGameObject->AddTexture(app->textures->LoadTexture("Assets/street_text2.png"));
	openGameObject->AddMesh(app->mesh3d->LoadFile("Assets/street.fbx", openGameObject));
	openGameObject->SetPos(vec3(0, 0.1f, 0));

	CreateAnimatedMoai();

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

void ModuleScene::CreateAnimatedMoai() {
	// Animated Moai
	GameObject* moai = new GameObject("Animated Moai", app->editor->root);
	moai->AddTexture(app->textures->LoadTexture("Assets/Moai_texture.png"));
	moai->AddMesh(app->mesh3d->LoadFile("Assets/Moai.fbx", moai));
	moai->SetScale(vec3(10, 10, 10));
	moai->SetPos(vec3(0, 2, 0));

	GameObject* cubeBody = new GameObject("Moai Body", moai);
	cubeBody->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/cube.fbx"));
	cubeBody->SetPos(vec3(0, 0, 0));
	cubeBody->SetScale(vec3(5, 10, 5));

	GameObject* cubeLeftArm = new GameObject("Moai Left Arm", cubeBody);
	cubeLeftArm->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/cube.fbx"));
	cubeLeftArm->SetPos(vec3(2, 0, 0));
	cubeLeftArm->SetScale(vec3(3, 5, 3));

	GameObject* cubeRightArm = new GameObject("Moai Right Arm", cubeBody);
	cubeRightArm->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/cube.fbx"));
	cubeRightArm->SetPos(vec3(-2, 0, 0));
	cubeRightArm->SetScale(vec3(3, 5, 3));

	GameObject* cubeLeftLeg = new GameObject("Moai Left Leg", cubeBody);
	cubeLeftLeg->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/cube.fbx"));
	cubeLeftLeg->SetPos(vec3(2, -2, 0));
	cubeLeftLeg->SetScale(vec3(3, 5, 3));

	GameObject* cubeRightLeg = new GameObject("Moai Right Leg", cubeBody);
	cubeRightLeg->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/cube.fbx"));
	cubeRightLeg->SetPos(vec3(-2, -2, 0));
	cubeRightLeg->SetScale(vec3(3, 5, 3));

	app->editor->SetSelectedGameObject(moai);
}