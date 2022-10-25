#include "Globals.h"
#include "Application.h"

#include "ModuleConfig.h"

#include "../External/parson/parson.h"

ModuleConfig::ModuleConfig(Application* app, bool start_enabled) : Module(app, start_enabled){}

ModuleConfig::~ModuleConfig(){}

bool ModuleConfig::Start()
{
	LOG("Starting Config");

	return true;
}


update_status ModuleConfig::PreUpdate(float dt) {


	return UPDATE_CONTINUE;
}

update_status ModuleConfig::Update(float dt) {

	return UPDATE_CONTINUE;
}

update_status ModuleConfig::PostUpdate(float dt) {

	return UPDATE_CONTINUE;
}

bool ModuleConfig::CleanUp() {

	return true;
}