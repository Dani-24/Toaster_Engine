#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "../External/SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	void SetTitle(const char* title);

	void GUIinfo();

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

private:

	int width, height;

	float brightness = 1;

	bool borderless = false;
	bool resizable = false;
	bool fullScreen = false;
	bool fullScreenDesktop = false;

	void SetSize(int w, int h);
	void GetSize(int& w, int& h);

	void WhyDoYouWantToSetBrightnessWTF(float b);

	void SetBorderless(bool border);
	bool IsBorderless() { return borderless; }

	void SetResizable(bool resi);
	bool IsResizable() { return resizable; }
	void OnResize(int w, int h);

	void SetFullScreen(bool full);
	bool IsFullScreen() { return fullScreen; }

	void SetFullScreenDesktop(bool fullD);
	bool IsFullScreenDesktop() { return fullScreenDesktop; }

	SDL_DisplayMode cum;
};

#endif // __ModuleWindow_H__