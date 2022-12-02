#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#include "Camera.h"

#define MAX_LIGHTS 8

class Hardware {
public:
	std::string caps;
	char SDLVersion[25] = "";

	uint CPUCount;
	uint CPUCache;
	float systemRAM;

	uint GPUVendor = 0;
	uint GPUDevice = 0;
	char GPUBrand[250] = "";
	float VRAMBudget = 0.f;
	float VRAMUsage = 0.f;
	float VRAMAvailable = 0.f;
	float VRAMReserved = 0.f;
};

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

	void GUIglInfo();
	void GUIhardwareInfo();

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	bool wireframe = false;

private:
	// Color
	Color currentColor, desiredColor;
	bool colorChanged, redChanged, greenChanged, blueChanged;
	float colorChangeSpeed = 0.005f;

	float lightIntensity = 255.0f;

	// GL Debug
	bool depthTest = true;
	bool cullFace = true;
	bool lighting = true;
	bool colorMaterial = true;
	bool texture2D = true;

	// Hardware
	Hardware hardware;

public:
	
	Camera* renderOnThisCamera;
};