#include "ModuleAnimation.h"
#include "Application.h"
#include "ModuleEditor.h"

#include "GameObject.h"
#include "C_Transform.h"
#include "C_Animator.h"

#include "R_Animation.h"

#include "ModuleScene.h"

#include "ModuleImporter.h"
#include "ModuleModelImporter.h"

//#include "MO_ResourceManager.h"

#include "../External/Assimp/include/cimport.h"
#include "../External/Assimp/include/scene.h"
#include "../External/Assimp/include/postprocess.h"
#include "../External/Assimp/include/cfileio.h"
#include "../External/Assimp/include/vector3.h"

#include "../External/MathGeoLib/include/Math/float3.h"

#pragma comment (lib, "External/Assimp/libx86/assimp-vc142-mt.lib")

#include "GameObject.h"

ResourceAnimation* ModuleAnimation::ImportAnimation(aiAnimation* importedAnimation, uint oldUID)
{
	uint UID = oldUID;
	if (UID == 0)
		UID = app->resourceManager->GenerateNewUID();

	ResourceAnimation* animation = dynamic_cast<ResourceAnimation*>(app->resourceManager->CreateNewResource("", UID, Resource::Type::ANIMATION));

	std::string animationName = importedAnimation->mName.C_Str();
	if (animationName.size() > 32)
		animationName.resize(32);

	strcpy(animation->animationName, animationName.c_str());
	animation->ticksPerSecond = importedAnimation->mTicksPerSecond;
	animation->duration = importedAnimation->mDuration;

	//Import from Assimp
	for (int i = 0; i < importedAnimation->mNumChannels; i++)
	{
		Channel channel;

		channel.boneName = importedAnimation->mChannels[i]->mNodeName.C_Str();

		uint pos = channel.boneName.find("_$AssimpFbx$_");
		if (pos != std::string::npos)
		{
			channel.boneName = channel.boneName.substr(0, pos);
		}
		for (int j = 0; j < importedAnimation->mChannels[i]->mNumPositionKeys; j++)
		{
			aiVector3D aiValue = importedAnimation->mChannels[i]->mPositionKeys[j].mValue;
			float3 positionKey = float3(aiValue.x, aiValue.y, aiValue.z);

			channel.positionKeys[importedAnimation->mChannels[i]->mPositionKeys[j].mTime] = positionKey;
		}

		for (int j = 0; j < importedAnimation->mChannels[i]->mNumRotationKeys; j++)
		{
			aiQuaternion aiValue = importedAnimation->mChannels[i]->mRotationKeys[j].mValue;
			Quat rotationKey = Quat(aiValue.x, aiValue.y, aiValue.z, aiValue.w);

			channel.rotationKeys[importedAnimation->mChannels[i]->mRotationKeys[j].mTime] = rotationKey;
		}

		for (int j = 0; j < importedAnimation->mChannels[i]->mNumScalingKeys; j++)
		{
			aiVector3D aiValue = importedAnimation->mChannels[i]->mScalingKeys[j].mValue;
			float3 scaleKey = float3(aiValue.x, aiValue.y, aiValue.z);

			channel.scaleKeys[importedAnimation->mChannels[i]->mScalingKeys[j].mTime] = scaleKey;
		}
		animation->channels[channel.boneName] = channel;
		//animation->channels.push_back(channel);
	}

	////Save animation own format
	//char* buffer;
	//uint size = animation->SaveCustomFormat(animation, &buffer);

	//RELEASE_ARRAY(buffer);

	return animation;
}

uint ModuleAnimation::GetChannelsSize(const Channel& channel)
{
	uint ret = sizeof(uint) + channel.boneName.size() + sizeof(uint) * 3;
	//Translation
	ret += sizeof(double) * channel.positionKeys.size() + sizeof(float) * channel.positionKeys.size() * 3;
	//Rotation
	ret += sizeof(double) * channel.rotationKeys.size() + sizeof(float) * channel.rotationKeys.size() * 4;
	//Scale
	ret += sizeof(double) * channel.scaleKeys.size() + sizeof(float) * channel.scaleKeys.size() * 3;

	return ret;
}

void ModuleAnimation::SaveChannels(const Channel& channel, char** cursor)
{
	uint nameSize = channel.boneName.size();
	memcpy(*cursor, &nameSize, sizeof(uint));
	*cursor += sizeof(uint);

	memcpy(*cursor, channel.boneName.c_str(), channel.boneName.size());
	*cursor += channel.boneName.size();

	//Ranges
	uint ranges[3] = { channel.positionKeys.size(), channel.rotationKeys.size(), channel.scaleKeys.size() };
	memcpy(*cursor, ranges, sizeof(uint) * 3);
	*cursor += sizeof(uint) * 3;

	//Save each channel, depending on float or quat based
	SaveChanKeys(channel.positionKeys, cursor);
	SaveChanKeys(channel.rotationKeys, cursor);
	SaveChanKeys(channel.scaleKeys, cursor);
}

void ModuleAnimation::SaveChanKeys(const std::map<double, float3>& map, char** cursor)
{
	std::map<double, float3>::const_iterator it = map.begin();
	for (it = map.begin(); it != map.end(); it++)
	{
		memcpy(*cursor, &it->first, sizeof(double));
		*cursor += sizeof(double);

		memcpy(*cursor, &it->second, sizeof(float) * 3);
		*cursor += sizeof(float) * 3;
	}
}

void ModuleAnimation::SaveChanKeys(const std::map<double, Quat>& map, char** cursor)
{
	std::map<double, Quat>::const_iterator it = map.begin();
	for (it = map.begin(); it != map.end(); it++)
	{
		memcpy(*cursor, &it->first, sizeof(double));
		*cursor += sizeof(double);

		memcpy(*cursor, &it->second, sizeof(float) * 4);
		*cursor += sizeof(float) * 4;
	}
}

void ModuleAnimation::LoadChannels(Channel& channel, const char** cursor)
{
	uint bytes = 0;

	uint nameSize = 0;
	memcpy(&nameSize, *cursor, sizeof(uint));
	*cursor += sizeof(uint);

	if (nameSize > 0)
	{
		char* string = new char[nameSize + 1];
		bytes = sizeof(char) * nameSize;
		memcpy(string, *cursor, bytes);
		*cursor += bytes;
		string[nameSize] = '\0';
		channel.boneName = string;
		RELEASE_ARRAY(string);
	}

	//Ranges
	uint ranges[3] = { 0,0,0 };
	memcpy(&ranges, *cursor, sizeof(uint) * 3);
	*cursor += sizeof(uint) * 3;

	LoadChanKeys(channel.positionKeys, cursor, ranges[0]);
	LoadChanKeys(channel.rotationKeys, cursor, ranges[1]);
	LoadChanKeys(channel.scaleKeys, cursor, ranges[2]);
}

void ModuleAnimation::LoadChanKeys(std::map<double, float3>& map, const char** cursor, uint size)
{
	for (uint i = 0; i < size; i++)
	{
		double time;
		memcpy(&time, *cursor, sizeof(double));
		*cursor += sizeof(double);
		float data[3];
		memcpy(&data, *cursor, sizeof(float) * 3);
		*cursor += sizeof(float) * 3;

		map[time] = float3(data);
	}
}

void ModuleAnimation::LoadChanKeys(std::map<double, Quat>& map, const char** cursor, uint size)
{
	for (uint i = 0; i < size; i++)
	{
		double time;
		memcpy(&time, *cursor, sizeof(double));
		*cursor += sizeof(double);
		float data[4];
		memcpy(&data, *cursor, sizeof(float) * 4);
		*cursor += sizeof(float) * 4;

		map[time] = Quat(data);
	}
}

void ModuleAnimation::SetAnimationOnGameObjectRoot(aiAnimation** animArray, std::vector<ResourceAnimation*>& _sceneAnimations, GameObject* gmRoot)
{
	for (int i = 0; i < _sceneAnimations.size(); i++)
	{
		aiAnimation* importedAnim = animArray[i];

		if (importedAnim->mDuration != 0)
		{
			C_Animator* animator = dynamic_cast<C_Animator*>(gmRoot->AddComponent(Component::TYPE::ANIMATOR));
		}
	}
}