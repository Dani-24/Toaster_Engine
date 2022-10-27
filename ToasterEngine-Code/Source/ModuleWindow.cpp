#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "OpenGL.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		// Create window
		int width = SCREEN_WIDTH * SCREEN_SIZE;
		int height = SCREEN_HEIGHT * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		// Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		// Refresh ratio
		for (int i = 0; i < SDL_GetNumVideoDisplays(); ++i) {

			int zeroChecker = SDL_GetCurrentDisplayMode(i, &cum);

			if (zeroChecker != 0) {
				SDL_Log("Could not get display mode for video display #%d: %s", i, SDL_GetError());
			}
			else {
				SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz.", i, cum.w, cum.h, cum.refresh_rate);
			}
		}

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);


		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	width = SCREEN_WIDTH;
	height = SCREEN_HEIGHT;

	return ret;
}

bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

void ModuleWindow::GUIinfo() {

	//ImGui::TextWrapped("Icon: icon1.ini "); // That should be a path?

	if (ImGui::SliderFloat("Brightness", &brightness, 0.f, 1.f))
	{
		WhyDoYouWantToSetBrightnessWTF(brightness);
	}
	if (ImGui::SliderInt("Width", &width, 640, cum.w))
	{
		SetSize(width, height);
	}
	if (ImGui::SliderInt("Height", &height, 480, cum.h))
	{
		SetSize(width, height);
	}

	ImGui::TextWrapped("Your physical toaster refresh rate: %d", cum.refresh_rate);

	if (ImGui::Checkbox("Fullscreen", &fullScreen)) {
		SetFullScreen(fullScreen);
		SDL_GetWindowSize(window, &width, &height);
		OnResize(width, height);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Change fullscreen mode");
	}

	ImGui::SameLine();

	if (ImGui::Checkbox("Fullscreen Desktop", &fullScreenDesktop)) {
		SetFullScreenDesktop(fullScreenDesktop);

		SDL_GetWindowSize(window, &width, &height);
		OnResize(width, height);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Change fullscreen desktop mode");
	}

	if (ImGui::Checkbox("Resizable", &resizable)) {
		if (!fullScreen && !fullScreenDesktop) SetResizable(resizable);
		else resizable = !resizable;
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Change resizable mode, only on Off FullScreen mode");
	}

	if (ImGui::Checkbox("Borderless", &borderless)) {
		if (!fullScreen && !fullScreenDesktop) SetBorderless(borderless);
		else borderless = !borderless;
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Change borderless mode, only on Off FullScreen mode");
	}

	if (ImGui::Checkbox("Wireframe", &app->renderer3D->wireframe)) {}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Change render mode");
	}

	ImGui::NewLine();
}

void ModuleWindow::SetSize(int w, int h) {
	width = w;
	height = h;
	SDL_SetWindowSize(window, w, h);
}
void ModuleWindow::GetSize(int& w, int& h) {
	w = width;
	h = height;
}

void ModuleWindow::WhyDoYouWantToSetBrightnessWTF(float b) {
	brightness = b;
	SDL_SetWindowBrightness(window, brightness);
}

void ModuleWindow::SetBorderless(bool border) {
	borderless = border;
	SDL_SetWindowBordered(window, (SDL_bool)!border);
}

void ModuleWindow::SetResizable(bool resi) {
	resizable = resi;
	SDL_SetWindowResizable(window, (SDL_bool)resi);
}

void ModuleWindow::OnResize(int w, int h) {
	glViewport(0, 0, w, h);

	width = w;
	height = h;
}

void ModuleWindow::SetFullScreen(bool full) {
	fullScreen = full;

	if (fullScreen) {
		fullScreenDesktop = false;
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	}
	else {
		SDL_SetWindowFullscreen(window, 0);
	}
}

void ModuleWindow::SetFullScreenDesktop(bool fullD) {
	fullScreenDesktop = fullD;

	if (fullScreenDesktop) {
		fullScreen = false;

		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	else {
		SDL_SetWindowFullscreen(window, 0);
	}
}