#pragma once

#include "Module.h"
#include "Globals.h"

#include <string>

struct aiAnimation;

struct Channel {

	std::string name;

	std::vector<float3> posKeys;
	std::vector<Quat> rotKeys;
	std::vector<float3> scaleKeys;

};

struct Animation {
	
	Animation(std::string name, float duration, float ticksPerSec) 
	{
		this->name = name;
		this->duration = duration;
		this->ticksPerSec = ticksPerSec;
	};

	std::string name;

	float duration;
	float ticksPerSec; 

	float currentFrame = 0;

	bool loop = true;

	std::vector<Channel> channels, bakedChannels;

};

class ModuleAnimation : public Module 
{
public:
	ModuleAnimation(Application* app, bool start_enabled = true);
	~ModuleAnimation() {};

	Animation* LoadAnimation(aiAnimation* anim);
};