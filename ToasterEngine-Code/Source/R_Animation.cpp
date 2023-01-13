#include "Application.h"
#include "Resource.h"

//#include "IM_FileSystem.h"
//#include "MO_ResourceManager.h"

#include "R_Animation.h"
#include "ModuleAnimation.h"

ResourceAnimation::ResourceAnimation(unsigned int _uid) : Resource(_uid, Resource::Type::ANIMATION), animationName("No Name"), duration(0), ticksPerSecond(0), loop(true), time(0.0f)
{
}

ResourceAnimation::~ResourceAnimation()
{}

bool ResourceAnimation::LoadToMemory()
{
	LOG("Animation loaded to memory");

	//LoadCustomFormat(GetLibraryPath());

	return true;
}

bool ResourceAnimation::UnloadFromMemory()
{
	channels.clear();

	return false;
}

Channel::~Channel()
{
	boneName.clear();
	positionKeys.clear();
	rotationKeys.clear();
	scaleKeys.clear();
}

std::map<double, float3>::const_iterator Channel::GetPrevPosKey(double currentKey) const
{
	std::map<double, float3>::const_iterator ret = positionKeys.lower_bound(currentKey);
	if (ret != positionKeys.begin())
		ret--;

	return ret;
}

std::map<double, float3>::const_iterator Channel::GetNextPosKey(double currentKey) const
{
	std::map<double, float3>::const_iterator ret = positionKeys.find(currentKey);
	if (ret == positionKeys.end()) {
		ret = positionKeys.upper_bound(currentKey);

		if (ret == positionKeys.end())
			ret--;
	}
	return ret;
}

std::map<double, Quat>::const_iterator Channel::GetPrevRotKey(double currentKey) const
{
	std::map<double, Quat>::const_iterator ret = rotationKeys.lower_bound(currentKey);
	if (ret != rotationKeys.begin())
		ret--;
	return ret;
}

std::map<double, Quat>::const_iterator Channel::GetNextRotKey(double currentKey) const
{
	std::map<double, Quat>::const_iterator ret = rotationKeys.find(currentKey);
	if (ret == rotationKeys.end()) {
		ret = rotationKeys.upper_bound(currentKey);

		if (ret == rotationKeys.end())
			ret--;
	}
	return ret;
}

std::map<double, float3>::const_iterator Channel::GetPrevScaleKey(double currentKey) const
{
	std::map<double, float3>::const_iterator ret = scaleKeys.lower_bound(currentKey);
	if (ret != scaleKeys.begin())
		ret--;
	return ret;
}

std::map<double, float3>::const_iterator Channel::GetNextScaleKey(double currentKey) const
{
	std::map<double, float3>::const_iterator ret = scaleKeys.find(currentKey);
	if (ret == scaleKeys.end()) {
		ret = scaleKeys.upper_bound(currentKey);

		if (ret == scaleKeys.end())
			ret--;
	}

	return ret;
}