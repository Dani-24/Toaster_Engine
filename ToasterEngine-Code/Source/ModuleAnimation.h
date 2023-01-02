#pragma once

#include "Module.h"
#include "Globals.h"

#include <string>

struct aiAnimation;

struct /*Disney*/Channel {

	std::string name;

	std::map<double, float3> posKeys;
	std::map<double, float3> rotKeys;
	std::map<double, float3> scaleKeys;

	//bool HasPosKey() const;
	std::map<double, float3>::const_iterator GetPrevPosKey(double currentKey) const;
	std::map<double, float3>::const_iterator GetNextPosKey(double currentKey) const;

	//bool HasRotKey() const;
	std::map<double, float3>::const_iterator GetPrevRotKey(double currentKey) const;
	std::map<double, float3>::const_iterator GetNextRotKey(double currentKey) const;

	//bool HasScaleKey() const;
	std::map<double, float3>::const_iterator GetPrevScaleKey(double currentKey) const;
	std::map<double, float3>::const_iterator GetNextScaleKey(double currentKey) const;
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

	std::map<std::string, Channel> channels, bakedChannels;

};

class ModuleAnimation : public Module 
{
public:
	ModuleAnimation(Application* app, bool start_enabled = true);
	~ModuleAnimation() {};

	// Load Animations from meshes (Using Assimp)
	Animation* LoadAnimation(aiAnimation* anim);

	// Animation Channels Stuff //

	// Name, Pos, Rot, Scale to uint
	uint ChannelSize(const Channel& ch);

	// Save Channels
	void SaveChannel(const Channel& ch, char** cursor);

	// Save each type of Channel (float3 / Quaternion)
	void SaveChannelKeys(const std::map<double, float3>& map, char** cursor);
	//void SaveChannelKeys(const std::map<double, Quat>& map, char** cursor);

	// Load Channels
	void LoadChannel(Channel& ch, const char** cursor);

	// Load each type of Channel (float3 / Quaternion)
	void LoadChannelKeys(std::map<double, float3>& map, const char** cursor, uint size);
	//void LoadChannelKeys(std::map<double, Quat>& map, const char** cursor, uint size);
};