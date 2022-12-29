#include "Application.h"

#include "ModuleAnimation.h"

#include "ModuleMesh3D.h"

ModuleAnimation::ModuleAnimation(Application* app, bool start_enabled) : Module(app, start_enabled) 
{
}

Animation* ModuleAnimation::LoadAnimation(aiAnimation* anim) {
		
	std::string animationName = anim->mName.C_Str();
		
	if (animationName.size() > 32) {
		animationName.resize(32);
	}

	Animation* animation = new Animation(animationName, anim->mDuration, anim->mTicksPerSecond);

	for (int i = 0; i < anim->mNumChannels; i++) {
		Channel channel;

		channel.name = anim->mChannels[i]->mNodeName.C_Str();

		uint pos = channel.name.find("_$AssimpFbx$_");
		if (pos != std::string::npos)
		{
			channel.name = channel.name.substr(0, pos);
		}
		for (int j = 0; j < anim->mChannels[i]->mNumPositionKeys; j++)
		{
			aiVector3D aiValue = anim->mChannels[i]->mPositionKeys[j].mValue;
			float3 positionKey = float3(aiValue.x, aiValue.y, aiValue.z);

			channel.posKeys[anim->mChannels[i]->mPositionKeys[j].mTime] = positionKey;
		}

		for (int j = 0; j < anim->mChannels[i]->mNumRotationKeys; j++)
		{
			aiQuaternion aiValue = anim->mChannels[i]->mRotationKeys[j].mValue;
			Quat rotationKey = Quat(aiValue.x, aiValue.y, aiValue.z, aiValue.w);

			channel.rotKeys[anim->mChannels[i]->mRotationKeys[j].mTime] = rotationKey;
		}

		for (int j = 0; j < anim->mChannels[i]->mNumScalingKeys; j++)
		{
			aiVector3D aiValue = anim->mChannels[i]->mScalingKeys[j].mValue;
			float3 scaleKey = float3(aiValue.x, aiValue.y, aiValue.z);

			channel.scaleKeys[anim->mChannels[i]->mScalingKeys[j].mTime] = scaleKey;
		}
		
		//animation->channels[channel.name] = channel;
		animation->channels.push_back(channel);
	}

	LOG("MESH 3D: Loaded %s Animation with %.2f duration.", animation->name.c_str(), animation->duration);

	return animation;
}