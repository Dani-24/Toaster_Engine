#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#include "OpenGL.h"

#include "../External/ImGui/imgui.h"
#include "../External/ImGui/backends/imgui_impl_sdl.h"
#include "../External/ImGui/backends/imgui_impl_opengl3.h"
#include <stdio.h>

#include "../External/GPUDetector/DeviceId.h"

#ifdef _DEBUG
#pragma comment (lib, "External/MathGeoLib/libx86/MathDebug/MathGeoLib.lib")
#else
#pragma comment (lib, "External/MathGeoLib/libx86/MathRelease/MathGeoLib.lib")
#endif

// JUST TESTING
#include "../External/PhysFS/include/physfs.h"
#pragma comment (lib, "External/PhysFS/libx86/physfs.lib")

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	hardware.caps += (SDL_HasRDTSC()) ? "RDTSC," : "";
	hardware.caps += (SDL_HasMMX()) ? "MMX, " : "";
	hardware.caps += (SDL_HasSSE()) ? "SSE, " : "";
	hardware.caps += (SDL_HasSSE2()) ? "SSE2, " : "";
	hardware.caps += (SDL_HasSSE3()) ? "SSE3, " : "";
	hardware.caps += "\n";
	hardware.caps += (SDL_HasSSE41()) ? "SSE41, " : "";
	hardware.caps += (SDL_HasSSE42()) ? "SSE42, " : "";
	hardware.caps += (SDL_HasAVX()) ? "AVX, " : "";
	hardware.caps += (SDL_HasAltiVec()) ? "AltiVec, " : "";
	hardware.caps += (SDL_Has3DNow()) ? "3DNow, " : "";

	SDL_version version;
	SDL_GetVersion(&version);
	sprintf_s(hardware.SDLVersion, 25, "%i.%i.%i", version.major, version.minor, version.patch);
	hardware.CPUCount = SDL_GetCPUCount();
	hardware.CPUCache = SDL_GetCPUCacheLineSize();
	hardware.systemRAM = SDL_GetSystemRAM() / 1024.f;

	uint vendor, deviceId;
	std::wstring brand;
	unsigned __int64 videoMemBudget, videoMemUsage, videoMemAvailable, videoMemReserved;

	if (getGraphicsDeviceInfo(&vendor, &deviceId, &brand, &videoMemBudget, &videoMemUsage, &videoMemAvailable, &videoMemReserved))
	{
		hardware.GPUVendor = vendor;
		hardware.GPUDevice = deviceId;
		sprintf_s(hardware.GPUBrand, 250, "%S", brand.c_str());
		hardware.VRAMBudget = float(videoMemBudget) / (1073741824.0f / 1024.f);
		hardware.VRAMUsage = float(videoMemUsage) / (1024.f * 1024.f * 1024.f);
		hardware.VRAMAvailable = float(videoMemAvailable) / (1024.f * 1024.f);
		hardware.VRAMReserved = float(videoMemReserved) / (1024.f * 1024.f * 1024.f);
	}
}

ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(app->window->window);
	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (ret == true)
	{
		//Use Vsync
		if (VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glewInit();

		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", glewGetErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", glewGetErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(Cyan.r, Cyan.g, Cyan.b, 1.f);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", glewGetErrorString(error));
			ret = false;
		}

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(1.0f, 1.0f, 1.0f, 1.0f);
		lights[0].diffuse.Set(1.0f, 1.0f, 1.0f, 1.0f);
		lights[0].SetPos(0.0f, 100.0f, 10.0f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		// COSAS COMO EL CULL (CORTE DE COSAS)
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glAlphaFunc(GL_GREATER, 0.5);
		glEnable(GL_ALPHA_TEST);

		lights[0].Active(true);
		lights[1].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);

		glEnable(GL_TEXTURE_2D);
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// IMGUI CONFIG Flags (Should be configured later from the Engine? meh who cares)

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		// Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;			// Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(app->window->window, context);
	ImGui_ImplOpenGL3_Init("#version 130");

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(app->camera->GetViewMatrix().ptr());

	// ====================================
	//				Lights	
	// ====================================

	// light 0 on cam pos
	lights[0].SetPos(app->camera->GetPos().x, app->camera->GetPos().y, app->camera->GetPos().z);

	for (uint i = 0; i < MAX_LIGHTS; ++i) {
		lights[i].Render();
	}

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(app->window->window);
	ImGui::NewFrame();

	// Dockspace
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNavFocus;

	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(main_viewport->WorkPos);
	ImGui::SetNextWindowSize(main_viewport->Size);
	ImGui::SetNextWindowViewport(main_viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("Docking", nullptr, flags);

	ImGui::PopStyleVar(3);

	ImGuiID dockspace_id = ImGui::GetID("ToastingSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

	ImGui::End();

	app->editor->Draw();

	// Rendering

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

	SDL_GL_SwapWindow(app->window->window);
	return UPDATE_CONTINUE;
}

//Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::SetBGColor(int R, int G, int B)
{
	glClearColor(R, G, B, 1.0f);
}

void ModuleRenderer3D::GUIglInfo() {
	if (ImGui::Checkbox("GL_DEPTH_TEST", &depthTest)) {
		if (depthTest) glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Enable/Disable GL_DEPTH_TEST");
	}

	if (ImGui::Checkbox("GL_CULL_FACE", &cullFace)) {
		if (cullFace) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
	}

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Enable/Disable GL_CULL_FACE");
	}

	if (ImGui::Checkbox("GL_COLOR_MATERIAL", &colorMaterial)) {
		if (colorMaterial) glEnable(GL_COLOR_MATERIAL);
		else glDisable(GL_COLOR_MATERIAL);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Enable/Disable GL_COLOR_MATERIAL");
	}

	if (ImGui::Checkbox("GL_TEXTURE_2D", &texture2D)) {
		if (texture2D) glEnable(GL_TEXTURE_2D);
		else glDisable(GL_TEXTURE_2D);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Enable/Disable GL_TEXTURE_2D");
	}

	if (ImGui::Checkbox("GL_LIGHTING", &lighting)) {
		if (lighting) glEnable(GL_LIGHTING);
		else glDisable(GL_LIGHTING);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Enable/Disable GL_LIGHTING");
	}

}

void ModuleRenderer3D::GUIhardwareInfo() 
{
	ImGui::TextWrapped("SDL Version: %s", hardware.SDLVersion);
	ImGui::TextWrapped("OpenGL Version: %s", glGetString(GL_VERSION));
	ImGui::TextWrapped("Glew Version: %s", glewGetString(GLEW_VERSION));

	ImGui::Separator();
	ImGui::TextWrapped("GLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
	ImGui::TextWrapped("Vendor: %s", glGetString(GL_VENDOR));
	ImGui::TextWrapped("Renderer: %s", glGetString(GL_RENDERER));

	ImGui::Separator();
	ImGui::TextWrapped("CPUs: %d (Cache: %dkb)", hardware.CPUCount, hardware.CPUCache);
	ImGui::TextWrapped("System RAM: %.1fGb", hardware.systemRAM);
	ImGui::TextWrapped("Caps: %s", hardware.caps.c_str());

	ImGui::Separator();
	ImGui::TextWrapped("GPU: vendor %u device %u", hardware.GPUVendor, hardware.GPUDevice);
	ImGui::TextWrapped("Brand: %s", hardware.GPUBrand);
	ImGui::TextWrapped("VRAM Budget: %.1f Mb", hardware.VRAMBudget);
	ImGui::TextWrapped("VRAM Usage: %.1f Mb", hardware.VRAMUsage);
	ImGui::TextWrapped("VRAM Available: %.1f Mb", hardware.VRAMAvailable);
	ImGui::TextWrapped("VRAM Reserved: %.1f Mb", hardware.VRAMReserved);
}