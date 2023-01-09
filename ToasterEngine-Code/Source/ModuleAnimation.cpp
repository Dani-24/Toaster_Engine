#include "Application.h"

#include "ModuleAnimation.h"

#include "ModuleMesh3D.h"

ModuleAnimation::ModuleAnimation(Application* app, bool start_enabled) : Module(app, start_enabled) 
{
}

// Loading from Assimp
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
			Quat q = Quat(aiValue.x, aiValue.y, aiValue.z, aiValue.w);

			float3 rotationKey;
			{
				// X
				double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
				double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
				rotationKey.x = std::atan2(sinr_cosp, cosr_cosp);

				// Y
				double sinp = std::sqrt(1 + 2 * (q.w * q.x - q.y * q.z));
				double cosp = std::sqrt(1 - 2 * (q.w * q.x - q.y * q.z));
				rotationKey.y = 2 * std::atan2(sinp, cosp) - M_PI / 2;

				// Z
				double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
				double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
				rotationKey.z = std::atan2(siny_cosp, cosy_cosp);
			}
			// Thank you Wikipedia

			channel.rotKeys[anim->mChannels[i]->mRotationKeys[j].mTime] = rotationKey;
		}

		for (int j = 0; j < anim->mChannels[i]->mNumScalingKeys; j++)
		{
			aiVector3D aiValue = anim->mChannels[i]->mScalingKeys[j].mValue;
			float3 scaleKey = float3(aiValue.x, aiValue.y, aiValue.z);

			channel.scaleKeys[anim->mChannels[i]->mScalingKeys[j].mTime] = scaleKey;
		}
		animation->channels[channel.name] = channel;

		GameObject* bone = new GameObject(channel.name.c_str(), app->editor->root);
	}

	LOG("MESH 3D: Loaded %s Animation with %.2f duration.", animation->name.c_str(), animation->duration);

	return animation;
}

// Position
std::map<double, float3>::const_iterator Channel::GetPrevPosKey(double currentKey) const {
	std::map<double, float3>::const_iterator ret = posKeys.lower_bound(currentKey);
	if (ret != posKeys.begin())
		ret--;

	return ret;
}

std::map<double, float3>::const_iterator Channel::GetNextPosKey(double currentKey) const
{
	return posKeys.upper_bound(currentKey);
}

// Rotation
std::map<double, float3>::const_iterator Channel::GetPrevRotKey(double currentKey) const
{
	std::map<double, float3>::const_iterator ret = rotKeys.lower_bound(currentKey);
	if (ret != rotKeys.begin())
		ret--;
	return ret;
}

std::map<double, float3>::const_iterator Channel::GetNextRotKey(double currentKey) const
{
	return rotKeys.upper_bound(currentKey);
}

// Scale
std::map<double, float3>::const_iterator Channel::GetPrevScaleKey(double currentKey) const
{
	std::map<double, float3>::const_iterator ret = scaleKeys.lower_bound(currentKey);
	if (ret != scaleKeys.begin())
		ret--;
	return ret;
}

std::map<double, float3>::const_iterator Channel::GetNextScaleKey(double currentKey) const
{
	return scaleKeys.upper_bound(currentKey);
}

// CHANNELS

uint ModuleAnimation::ChannelSize(const Channel& ch) {
	
	uint ret = sizeof(uint) + ch.name.size() + sizeof(uint) * 3;

	ret += sizeof(double) * ch.posKeys.size() + sizeof(float) * ch.posKeys.size() * 3;
	ret += sizeof(double) * ch.rotKeys.size() + sizeof(float) * ch.rotKeys.size() * 4;
	ret += sizeof(double) * ch.scaleKeys.size() + sizeof(float) * ch.scaleKeys.size() * 3;

	return ret;
}

void ModuleAnimation::SaveChannel(const Channel& ch, char** cursor) {
	
	// Set Name
	uint nameSize = ch.name.size();
	memcpy(*cursor, &nameSize, sizeof(uint));
	*cursor += sizeof(uint);

	memcpy(*cursor, ch.name.c_str(), ch.name.size());
	*cursor += ch.name.size();

	// Set Range
	uint range[3] = { ch.posKeys.size(), ch.rotKeys.size(), ch.scaleKeys.size() };
	memcpy(*cursor, range, sizeof(uint) * 3);
	*cursor += sizeof(uint) * 3;

	// Save Channels
	SaveChannelKeys(ch.posKeys, cursor);
	SaveChannelKeys(ch.rotKeys, cursor);
	SaveChannelKeys(ch.scaleKeys, cursor);
}

void ModuleAnimation::SaveChannelKeys(const std::map<double, float3>& map, char** cursor) {
	std::map<double, float3>::const_iterator it = map.begin();

	for (it = map.begin(); it != map.end(); it++)
	{
		memcpy(*cursor, &it->first, sizeof(double));
		*cursor += sizeof(double);

		memcpy(*cursor, &it->second, sizeof(float) * 3);
		*cursor += sizeof(float) * 3;
	}
}

//void ModuleAnimation::SaveChannelKeys(const std::map<double, Quat>& map, char** cursor) {
//	std::map<double, Quat>::const_iterator it = map.begin();
//
//	for (it = map.begin(); it != map.end(); it++)
//	{
//		memcpy(*cursor, &it->first, sizeof(double));
//		*cursor += sizeof(double);
//
//		memcpy(*cursor, &it->second, sizeof(float) * 4);
//		*cursor += sizeof(float) * 4;
//	}
//}

void ModuleAnimation::LoadChannel(Channel& ch, const char** cursor) 
{
	uint bytes = 0;

	// Get Name Size
	uint nameSize = 0;
	memcpy(&nameSize, *cursor, sizeof(uint));
	*cursor += sizeof(uint);

	// Get Name
	if (nameSize > 0)
	{
		char* string = new char[nameSize + 1];
		bytes = sizeof(char) * nameSize;

		memcpy(string, *cursor, bytes);
		*cursor += bytes;
		string[nameSize] = '\0';

		ch.name = string;

		RELEASE_ARRAY(string);
	}

	// Load Range
	uint range[3];
	memcpy(&range, *cursor, sizeof(uint) * 3);
	*cursor += sizeof(uint) * 3;

	LoadChannelKeys(ch.posKeys, cursor, range[0]);
	LoadChannelKeys(ch.rotKeys, cursor, range[1]);
	LoadChannelKeys(ch.scaleKeys, cursor, range[2]);
}

void ModuleAnimation::LoadChannelKeys(std::map<double, float3>& map, const char** cursor, uint size) {
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

//void ModuleAnimation::LoadChannelKeys(std::map<double, Quat>& map, const char** cursor, uint size) {
//	for (uint i = 0; i < size; i++)
//	{
//		double time;
//		memcpy(&time, *cursor, sizeof(double));
//		*cursor += sizeof(double);
//		float data[4];
//		memcpy(&data, *cursor, sizeof(float) * 4);
//		*cursor += sizeof(float) * 4;
//
//		map[time] = Quat(data);
//	}
//}