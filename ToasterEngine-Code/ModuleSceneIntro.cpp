#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"

#include "MathGeoLib/include/MathGeoLib.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Scene assets");
	bool ret = true;

	return ret;
}

update_status ModuleSceneIntro::Update(float dt)
{

	//LOG("%d", RandomIntValue());

	return UPDATE_CONTINUE;
}

bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}