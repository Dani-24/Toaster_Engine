#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	// Instantly Color Change
	void SetBGColor(int R, int G, int B);

	void DrawCube(float4x4 cube);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

private:
	// Color
	Color currentColor, desiredColor;
	bool colorChanged, redChanged, greenChanged, blueChanged;
	float colorChangeSpeed = 0.005f;

	float lightIntensity = 0.9f;
};