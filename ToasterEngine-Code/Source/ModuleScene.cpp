#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Primitive.h"

#include "../External/MathGeoLib/include/MathGeoLib.h"

#include "OpenGL.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Scene assets");
	bool ret = true;

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