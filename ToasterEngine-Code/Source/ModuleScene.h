#pragma once
#include "Module.h"
#include "Globals.h"

#include "Primitive.h"

struct f3line {
	float3 line;
	Color color;
};

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void AddLines(float3 line, Color color);
	void DrawLines();

public:
	bool axis = true;

	std::vector<f3line> lines;
};