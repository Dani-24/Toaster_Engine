#pragma once

#include "../External/MathGeoLib/include/MathGeoLib.h"
#include "../External/SDL/include/SDL.h"

class Application;

class Module
{
private :
	bool enabled;

public:
	Application* app;

	Module(Application* parent, bool start_enabled = true) : app(parent)
	{}

	virtual ~Module()
	{}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual int RandomIntValue(int min, int max) {
		LCG rand;

		return rand.Int(min, max);
	}

	virtual void OpenURL(const char* link) {
		SDL_OpenURL(link);
	}

};