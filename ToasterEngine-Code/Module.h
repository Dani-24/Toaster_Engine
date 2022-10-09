#pragma once

#include "MathGeoLib/include/MathGeoLib.h"
#include "SDL/include/SDL.h"

class Application;
struct PhysBody3D;

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

	virtual void OnCollision(PhysBody3D* body1, PhysBody3D* body2)
	{}

	virtual int RandomIntValue() {
		LCG rand;
		return rand.Int();
	}

	virtual void OpenURL(const char* link) {
		SDL_OpenURL(link);
	}

};