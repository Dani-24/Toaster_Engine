#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Primitive.h"

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

	GameObject* house = new GameObject("Baker House", app->editor->root);
	house->AddMesh(app->mesh3d->LoadFile("Assets/BakerHouse.fbx"));
	house->AddTexture(app->textures->ImportTexture("Assets/Baker_house.png"));

	return ret;
}

update_status ModuleScene::PreUpdate(float dt) {


	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{

	//LOG("%d", RandomIntValue());

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt) {

	if (axis) {
		PlanePrimitive p(0, 1, 0, 0);
		p.axis = true;
		p.Render();
	}

	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	LOG("Unloading scene");

	return true;
}