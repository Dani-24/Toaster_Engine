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

	CreateSkybox();

	CreateStreet();

	CreateAnimatedMoai();

	// AUDIO

	initSFX = app->audio->LoadFx("Toaster Bell.wav");

	initSFX2 = app->audio->LoadFx("Toaster Pushed Down.wav");

	attackSFX = app->audio->LoadFx("superiorZas.wav");
	
	app->audio->PlayFx(initSFX);

	app->audio->PlayFx(initSFX2);

	//app->audio->PlayMusic("Splatoon 3 Alterna Mission 8.ogg");

	return ret;
}

update_status ModuleScene::PreUpdate(float dt) {

	// Attack Animation
	if (app->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		app->audio->PlayFx(attackSFX);
	}
	// Walk Animation
	if (app->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		app->audio->PlayMusic("Lifes Incredible Again.ogg");
	}
	if (app->input->GetKey(SDL_SCANCODE_2) == KEY_UP) {
		app->audio->PlayMusic("Splatoon 3 Alterna Mission 8.ogg");
	}

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

void ModuleScene::CreateSkybox() {
	GameObject* skybox = new GameObject("skybox", nullptr);
	skybox->AddTexture(app->textures->LoadTexture("Assets/default_Meshes/skybox.png"));
	skybox->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/skybox.fbx", skybox));
	skybox->SetScale(vec3(200, 200, 200));
	skybox->SetRot(vec3(90, 0, 0));
	skybox->alwaysRender = true;
	skybox->aabb.SetPos(float3(2000, 2000, 2000));
}

void ModuleScene::CreateStreet() {
	GameObject* openGameObject = new GameObject("Demo Street Environment", app->editor->root);
	openGameObject->AddTexture(app->textures->LoadTexture("Assets/street_text1.png"));
	openGameObject->AddTexture(app->textures->LoadTexture("Assets/street_text2.png"));
	openGameObject->AddMesh(app->mesh3d->LoadFile("Assets/street.fbx", openGameObject));
	openGameObject->SetPos(vec3(0, 0.1f, 0));
}

void ModuleScene::CreateAnimatedMoai() {

	// Animated Moai
	moai = new GameObject("Animated Moai", app->editor->root);
	moai->AddTexture(app->textures->LoadTexture("Assets/Moai_texture.png"));
	moai->AddMesh(app->mesh3d->LoadFile("Assets/Moai.fbx", moai));
	//moai->SetScale(vec3(2, 2, 2));
	moai->SetPos(vec3(0, 9.1, 0));

	cubeBody = new GameObject("Moai Body", moai);
	cubeBody->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/cube.fbx"));
	cubeBody->AddTexture(app->textures->LoadTexture("Assets/cube.png"));
	cubeBody->SetPos(vec3(0, -2, 0));
	cubeBody->SetScale(vec3(1, 2, 1));

	cubeLeftArm = new GameObject("Moai Left Arm", cubeBody);
	cubeLeftArm->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/cube.fbx"));
	cubeLeftArm->AddTexture(app->textures->LoadTexture("Assets/cube.png"));
	cubeLeftArm->SetPos(vec3(2, 0, 1));
	cubeLeftArm->SetScale(vec3(0.5, 0.25, 2));

	cubeRightArm = new GameObject("Moai Right Arm", cubeBody);
	cubeRightArm->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/cube.fbx"));
	cubeRightArm->AddTexture(app->textures->LoadTexture("Assets/cube.png"));
	cubeRightArm->SetPos(vec3(-2, 0, 1));
	cubeRightArm->SetScale(vec3(0.5, 0.25, 2));

	cubeLeftLeg = new GameObject("Moai Left Leg", cubeBody);
	cubeLeftLeg->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/cube.fbx"));
	cubeLeftLeg->AddTexture(app->textures->LoadTexture("Assets/cube.png"));
	cubeLeftLeg->SetPos(vec3(2, -2, 0));
	cubeLeftLeg->SetScale(vec3(1, 1.5, 1));

	cubeRightLeg = new GameObject("Moai Right Leg", cubeBody);
	cubeRightLeg->AddMesh(app->mesh3d->LoadFile("Assets/default_Meshes/cube.fbx"));
	cubeRightLeg->AddTexture(app->textures->LoadTexture("Assets/cube.png"));
	cubeRightLeg->SetPos(vec3(-2, -2, 0));
	cubeRightLeg->SetScale(vec3(1, 1.5, 1));

	moai->animatedTransform = true;
	moai->rootBone = cubeBody;

	app->editor->SetSelectedGameObject(moai);

	TransAnimationClip* iddle = new TransAnimationClip();

	iddle->name = "Iddle Moai";
	iddle->loop = true;

	iddle->moaiMov.rotation.y = 1;

	iddle->cubeLeftArmMov.rotation.y = -1;
	iddle->cubeRightArmMov.rotation.y = 1;
	iddle->cubeLeftLegMov.rotation.z = -1;
	iddle->cubeRightLegMov.rotation.z = 1;

	iddle->endFrame = 500;

	TransAnimationClip* walk = new TransAnimationClip();

	walk->name = "Walking Moai";

	walk->cubeLeftArmMov.rotation.x = -1;
	walk->cubeRightArmMov.rotation.x = -1;
	walk->cubeLeftLegMov.rotation.x = 1;
	walk->cubeRightLegMov.rotation.x = 1;

	walk->endFrame = 300;

	TransAnimationClip* kick = new TransAnimationClip();

	kick->name = "Kick Moai";

	kick->cubeLeftArmMov.rotation.x = -1;
	kick->cubeBodyMov.rotation.y = -1;

	kick->endFrame = 60;
}