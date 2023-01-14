#include "C_Animator.h"
#include "R_Animation.h"

#include "OpenGL.h"

#include "ModuleInput.h"

#include "Application.h"

#include "ModuleRenderer3D.h"

#include "ModuleImporter.h"
#include "ResourceManager.h"

#include "GameObject.h"
#include "C_Material.h"
#include "C_Transform.h"
#include "C_Camera.h"
#include "C_Mesh.h"

#include "../External/ImGui/imgui.h"

#include "../External/MathGeoLib/include/Geometry/Frustum.h"
#include "../External/MathGeoLib/include/Geometry/Plane.h"

#include "DtTimer.h"

C_Animator::C_Animator(GameObject* _gameobject) : Component(_gameobject),
selectedClip(nullptr), currentAnimation(nullptr), previousAnimation(nullptr),
rootBoneUID(0u), meshRendererUID(0u), currentTimeAnimation(0u), previousTimeAnimation(0u),
prevAnimTime(0.0f), time(0.0f), blendTime(0.0f), blendTimeDuration(0.0f), defaultBlend(0.2f),
playing(false), started(false), showBones(false), animSpeed(1.0f)
{
	gameObject = _gameobject;
	name = "Animator";
}

C_Animator::~C_Animator()
{
	rootBone = nullptr;

	currentAnimation = nullptr;
	previousAnimation = nullptr;
	selectedClip = nullptr;

	for (std::map<std::string, ResourceAnimation*>::iterator it = animations.begin(); it != animations.end(); it++)
	{
		app->resourceManager->UnloadResource(it->second->GetUID());
		it->second = nullptr;
	}

	animations.clear();
	boneMapping.clear();
	clips.clear();
	currentAnimationLUT.clear();
	previousAnimationLUT.clear();
}

void C_Animator::Start()
{
	if (rootBone == nullptr)
	{
		if (!FindRootBone())
			return;
	}

	if (animations.size() > 0)
	{
		if (currentAnimation == nullptr)
		{
			Play(animations.begin()->first, defaultBlend);
		}
	}

	started = true;
}

void C_Animator::Update()
{
	float dt = DETime::deltaTime * animSpeed;

	if (rootBone == nullptr)
		FindRootBone();

	if (DETime::state == GameState::PLAY)
	{
		if (!started)
			Start();
	}
	else
	{
		return;
	}

	if (rootBone != nullptr)
	{
		if (showBones)
			DrawBones();

		if (currentAnimation != nullptr)
		{
			//Updating animation blend
			float blendRatio = 0.0f;
			if (blendTimeDuration > 0.0f)
			{
				prevAnimTime += dt;
				blendTime += dt;

				if (blendTime >= blendTimeDuration)
				{
					blendTimeDuration = 0.0f;
				}
				else if (previousAnimation && prevAnimTime >= previousAnimation->duration)
				{
					if (previousAnimation->loop == true)
					{
						prevAnimTime = 0.0f;
					}
				}

				if (blendTimeDuration > 0.0f)
					blendRatio = blendTime / blendTimeDuration;
			}
			//Endof Updating animation blend

			time += dt;
			currentTimeAnimation = time * currentAnimation->ticksPerSecond;
			currentTimeAnimation += currentAnimation->initTimeAnim;
			if (currentAnimation && currentTimeAnimation >= currentAnimation->duration) {
				if (currentAnimation->loop == true) {
					time = 0.0f;
				}
			}
			UpdateChannelsTransform(currentAnimation, blendRatio > 0.0f ? previousAnimation : nullptr, blendRatio);
		}
	}
}

void C_Animator::OnRecursiveUIDChange(std::map<uint, GameObject*> gameObjects)
{
	if (rootBoneUID != 0u)
	{
		GameObject* _rootBone = nullptr;
		std::map<uint, GameObject*>::iterator boneIt = gameObjects.begin();
		for (boneIt; boneIt != gameObjects.end(); ++boneIt)
		{
			if (boneIt->second->UID == rootBoneUID)
				rootBone = _rootBone;
		}

		if (rootBone == nullptr)
		{
			boneIt = gameObjects.find(rootBoneUID);

			if (boneIt != gameObjects.end())
				rootBone = boneIt->second;

		}

		//std::map<uint, GameObject*>::iterator boneIt = gameObjects.find(rootBoneUID);
		if (rootBone != nullptr)
		{
			rootBoneUID = rootBone->UID;

			boneMapping.clear();
			StoreBoneMapping(rootBone);

			if (meshRendererUID != 0u)
			{
				GameObject* meshRendererObject = nullptr;
				std::map<uint, GameObject*>::iterator meshRendererIt = gameObjects.find(meshRendererUID);

				if (meshRendererIt == gameObjects.end())
				{
					for (meshRendererIt = gameObjects.begin(); meshRendererIt != gameObjects.end(); ++meshRendererIt)
					{
						if (meshRendererIt->second->UID == meshRendererUID)
						{
							meshRendererObject = meshRendererIt->second;
							break;
						}
					}
				}
				else
				{
					meshRendererObject = meshRendererIt->second;
				}

				if (meshRendererObject != nullptr)
				{
					C_Mesh* mesh = dynamic_cast<C_Mesh*>(meshRendererObject->GetComponent(Component::TYPE::MESH));
					if (meshRenderer != nullptr)
					{
						meshRenderer->SetRootBone(rootBone);
						meshRendererUID = meshRendererObject->UID;
					}
				}
			}
		}
	}
}

ResourceAnimation* C_Animator::GetAnimation(const char* animationName)
{
	std::map<std::string, ResourceAnimation*>::iterator animation = animations.find(std::string(animationName));

	if (animation != animations.end())
		return animation->second;
	else
		return nullptr;
}

bool C_Animator::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Checkbox("Show Bones", &showBones);
		// RootBone and MeshRenderer =====================================================================================================

		if (rootBone == nullptr)
		{
			ImGui::Text("Drop a GameObject here to set the root bone:");
			ImGui::Button("Root Bone not set");
		}
		else {
			ImGui::Text("Root Bone: ");
			ImGui::SameLine();
			ImGui::Button(rootBone->name.c_str());
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_GAMEOBJECT"))
			{
				int uid = *(int*)payload->Data;

				/*GameObject* dropGO = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, uid);
				rootBone = dropGO;*/

			}
			ImGui::EndDragDropTarget();
		}
		ImGui::Spacing();


		if (meshRendererUID == 0u) {
			ImGui::Button("Drop here to set mesh renderer object");
		}
		else {
			ImGui::Text("MeshRenderer Game Object UID: ");
			char uidText[16];
			sprintf_s(uidText, "%d", meshRendererUID);
			ImGui::SameLine();
			ImGui::Button(uidText);
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_GAMEOBJECT"))
			{
				int uid = *(int*)payload->Data;

				/*GameObject* dropGO = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, uid);

				if (rootBone != nullptr) {
					C_Meshr* meshRenderer = dynamic_cast<C_Mesh*>(dropGO->GetComponent(Component::TYPE::MESH));
					if (meshRenderer != nullptr) {
						meshRenderer->SetRootBone(rootBone);
						meshRendererUID = dropGO->UID;
					}
				}*/
			}
			ImGui::EndDragDropTarget();
		}

		//=======================================================================================================================================

		if (currentAnimation == nullptr) {
			ImGui::Text("Current Animation: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "None");
		}
		else {
			ImGui::Text("Current Animation: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", currentAnimation->animationName);
			ImGui::Text("Duration: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%.2f", currentAnimation->duration);
			ImGui::Text("Ticks per second: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%.2f", currentAnimation->ticksPerSecond);
		}

		//List of existing animations
		static char newName[64];

		std::string animation_to_remove = "";
		ImGui::Text("Select a new animation");
		for (std::map<std::string, ResourceAnimation*>::iterator it = animations.begin(); it != animations.end(); ++it)
		{
			std::string animName = it->first;

			if (currentAnimation == it->second) {
				animName += " (Current)";
			}

			if (ImGui::Button(animName.c_str())) {
				Play(animName, defaultBlend);
				time = 0.f;
				sprintf_s(newName, animName.c_str());
			}

			ImGui::SameLine();
			ImGui::PushID(it->second->GetUID());
			if (ImGui::Button("Remove Animation"))
			{
				animation_to_remove = it->first;
				if (it->second == currentAnimation)
					currentAnimation = nullptr;
			}
			ImGui::PopID();
		}
		if (animation_to_remove.size() > 0)
		{
			app->resourceManager->UnloadResource(animations[animation_to_remove]->GetUID());
			animations.erase(animation_to_remove);
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Button("Drop new animation here");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_ANIMATION"))
			{
				std::string* path = (std::string*)payload->Data;

				ResourceAnimation* droppedAnimation = nullptr;

				if (path->find("Assets") != std::string::npos)
				{
					droppedAnimation = dynamic_cast<ResourceAnimation*>(app->resourceManager->RequestFromAssets(path->c_str()));
				}
				else //Take from Library
				{
					std::string uid;

					app->importer->GetFileName(path->c_str(), false);

					droppedAnimation = dynamic_cast<ResourceAnimation*>(app->resourceManager->RequestResource(std::atoi(uid.c_str()), Resource::Type::ANIMATION));
				}


				if (droppedAnimation != nullptr) {
					AddAnimation(droppedAnimation);
				}

			}
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (currentAnimation != nullptr)
		{
			ImGui::InputText("Name", newName, IM_ARRAYSIZE(newName));
			ImGui::Checkbox("Loop", &currentAnimation->loop);

			if (ImGui::Button("Save Animation")) {
				SaveAnimation(currentAnimation, newName);
			}
			ImGui::Separator();
		}

		ImGui::Text("Previous Animation Time: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%.2f", prevAnimTime);
		ImGui::Text("Current Animation Time: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", currentTimeAnimation);
		ImGui::Text("blendTime: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%.2f", blendTime);

		ImGui::Spacing();
		if (playing)
		{
			ImGui::Text("Playing: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "true");
		}
		else
		{
			ImGui::Text("Playing: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "false");
		}
		ImGui::Spacing();

		// Clips ===============================================================================================

		//Table header
		ImGui::Columns(3, "Clips");
		ImGui::Separator();
		ImGui::Text("Clips");
		ImGui::NextColumn();
		ImGui::Text("Start");
		ImGui::NextColumn();
		ImGui::Text("End");
		ImGui::NextColumn();
		ImGui::Separator();

		for (size_t i = 0; i < clips.size(); i++)
		{
			if (ImGui::Button(clips[i].name)) {
				selectedClip = &clips[i];
			}
			ImGui::NextColumn();
			ImGui::InputFloat("##start", &clips[i].startFrame, 0.0f, 0.0f, 0);
			ImGui::NextColumn();
			ImGui::InputFloat("##end", &clips[i].endFrame, 0.0f, 0.0f, 0);
			ImGui::NextColumn();
		}

		ImGui::Separator();
		ImGui::Columns(1);
		ImGui::Spacing();

		if (ImGui::Button("+"))
		{
			if (currentAnimation != nullptr) {
				AddClip(currentAnimation);
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("-"))
		{
			if (selectedClip != nullptr)
			{
				std::vector<AnimationClip> remainingClips;
				for (size_t i = 0; i < clips.size(); i++)
				{
					if (selectedClip != &clips[i])
						remainingClips.push_back(clips[i]);
				}

				selectedClip = nullptr;
				clips = remainingClips;
				remainingClips.clear();
			}
		}

		ImGui::Spacing();

		if (selectedClip != nullptr)
		{
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Text("Selected Clip");
			ImGui::InputText("Name", selectedClip->name, IM_ARRAYSIZE(selectedClip->name));
			ImGui::InputFloat("Start Frame", &selectedClip->startFrame, 1.0f, 0.0f);
			ImGui::InputFloat("End Frame", &selectedClip->endFrame, 1.0f, 0.0f);
			ImGui::Checkbox("Loop", &selectedClip->loop);
		}

		ImGui::Spacing();

		if (ImGui::Button("Apply")) {
			for (size_t i = 0; i < clips.size(); i++) {
				ResourceAnimation* animation = ClipToAnimation(clips[i]);
				AddAnimation(animation);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			selectedClip = nullptr;
			clips.clear();
		}

		ImGui::Spacing();
	}

	return true;
}

void C_Animator::StoreBoneMapping(GameObject* gameObject)
{
	boneMapping[gameObject->name] = dynamic_cast<C_Transform*>(gameObject->GetComponent(Component::TYPE::TRANSFORM));

	for (int i = 0; i < gameObject->children.size(); i++)
	{
		StoreBoneMapping(gameObject->children[i]);
	}
}

void C_Animator::Play(std::string animName, float blendDuration, float speed)
{
	std::map<std::string, ResourceAnimation*>::iterator it = animations.find(animName);
	if (it == animations.end())
		return;

	previousAnimation = currentAnimation;
	prevAnimTime = time;
	currentAnimation = animations[animName];
	blendTimeDuration = blendDuration;
	blendTime = 0.0f;
	time = 0;
	animSpeed = speed;

	SetAnimationLookUpTable(currentAnimation, currentAnimationLUT);
	SetAnimationLookUpTable(previousAnimation, previousAnimationLUT);
}

void C_Animator::Pause()
{
	active = false;
}

void C_Animator::Resume()
{
	active = true;
}

void C_Animator::SetSpeed(float speed)
{
	animSpeed = speed;
}

std::string C_Animator::GetCurrentAnimation()
{
	if (currentAnimation != nullptr) {
		return currentAnimation->animationName;
	}
	return "";
}

void C_Animator::AddAnimation(ResourceAnimation* anim)
{
	if (anim == nullptr)
		return;

	animations[anim->animationName] = anim;

	OrderAnimation(anim);
}

void C_Animator::AddClip(ResourceAnimation* anim)
{
	if (anim == nullptr)
		return;

	AnimationClip clip;

	strcpy(clip.name, anim->animationName);
	clip.startFrame = anim->initTimeAnim;
	clip.endFrame = anim->initTimeAnim + anim->duration;
	clip.originalAnimation = anim;

	clips.push_back(clip);
}

void C_Animator::UpdateChannelsTransform(const ResourceAnimation* animation, const ResourceAnimation* blend, float blendRatio)
{
	uint currentFrame = currentTimeAnimation;

	uint prevBlendFrame = 0;
	if (blend != nullptr)
	{
		prevBlendFrame = (blend->ticksPerSecond * prevAnimTime) + blend->initTimeAnim;
	}

	//LOG(LogType::L_NORMAL, "%i", currentFrame);
	std::map<C_Transform*, Channel*>::iterator boneIt;
	for (boneIt = currentAnimationLUT.begin(); boneIt != currentAnimationLUT.end(); ++boneIt)
	{
		Channel& channel = *boneIt->second;

		float3 position = GetChannelPosition(channel, currentFrame, boneIt->first->position);
		Quat   rotation = GetChannelRotation(channel, currentFrame, boneIt->first->rotation);
		float3 scale = GetChannelScale(channel, currentFrame, boneIt->first->localScale);

		//BLEND
		if (blend != nullptr)
		{
			std::map<C_Transform*, Channel*>::iterator foundChannel = previousAnimationLUT.find(boneIt->first);
			if (foundChannel != previousAnimationLUT.end()) {
				const Channel& blendChannel = *foundChannel->second;

				position = float3::Lerp(GetChannelPosition(blendChannel, prevBlendFrame, boneIt->first->position), position, blendRatio);
				rotation = Quat::Slerp(GetChannelRotation(blendChannel, prevBlendFrame, boneIt->first->rotation), rotation, blendRatio);
				scale = float3::Lerp(GetChannelScale(blendChannel, prevBlendFrame, boneIt->first->localScale), scale, blendRatio);
			}
		}

		boneIt->first->position = position;
		boneIt->first->eulerRotation = rotation.ToEulerXYZ() * RADTODEG;
		boneIt->first->localScale = scale;
		boneIt->first->updateTransform = true;
	}
}

float3 C_Animator::GetChannelPosition(const Channel& channel, float currentKey, float3 position) const
{
	if (channel.positionKeys.size() > 0)
	{
		std::map<double, float3>::const_iterator previous = channel.GetPrevPosKey(currentKey);
		std::map<double, float3>::const_iterator next = channel.GetNextPosKey(currentKey);

		if (channel.positionKeys.begin()->first == -1)
			return position;

		//If both keys are the same, no need to blend
		if (previous == next)
			position = previous->second;
		else //blend between both keys
		{
			//0 to 1
			float ratio = (currentKey - previous->first) / (next->first - previous->first);
			position = previous->second.Lerp(next->second, ratio);
		}
	}

	return position;
}

Quat C_Animator::GetChannelRotation(const Channel& channel, float currentKey, Quat rotation) const
{
	if (channel.rotationKeys.size() > 0)
	{
		std::map<double, Quat>::const_iterator previous = channel.GetPrevRotKey(currentKey);
		std::map<double, Quat>::const_iterator next = channel.GetNextRotKey(currentKey);

		if (channel.rotationKeys.begin()->first == -1)
			return rotation;

		//If both keys are the same, no need to blend
		if (previous == next)
			rotation = previous->second;
		else //blend between both keys
		{
			//0 to 1
			float ratio = (currentKey - previous->first) / (next->first - previous->first);
			rotation = previous->second.Slerp(next->second, ratio);
		}
	}
	return rotation;
}

float3 C_Animator::GetChannelScale(const Channel& channel, float currentKey, float3 scale) const
{
	if (channel.scaleKeys.size() > 0)
	{
		std::map<double, float3>::const_iterator previous = channel.GetPrevScaleKey(currentKey);
		std::map<double, float3>::const_iterator next = channel.GetPrevScaleKey(currentKey);

		if (channel.scaleKeys.begin()->first == -1)
			return scale;

		//If both keys are the same, no need to blend
		if (previous == next)
			scale = previous->second;
		else //blend between both keys
		{
			//0 to 1
			float ratio = (currentKey - previous->first) / (next->first - previous->first);
			scale = previous->second.Lerp(next->second, ratio);
		}
	}
	return scale;
}

bool C_Animator::FindRootBone()
{
	bool ret = true;
	if (rootBoneUID != 0u)
	{
		rootBone = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, rootBoneUID);

		if (rootBone == nullptr)
		{
			rootBoneUID = 0u;
			ret = false;
		}
		else
		{
			boneMapping.clear();
			StoreBoneMapping(rootBone);

			if (meshRendererUID != 0u)
			{
				GameObject* meshRendererObject = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, meshRendererUID);
				if (meshRendererObject != nullptr)
				{
					dynamic_cast<C_Mesh*>(meshRendererObject->GetComponent(Component::TYPE::MESH))->SetRootBone(rootBone);
				}
			}
		}
	}

	return ret;
}

void C_Animator::SetAnimationLookUpTable(ResourceAnimation* animation, std::map<C_Transform*, Channel*>& lookUpTable)
{
	if (animation == nullptr)
		return;

	lookUpTable.clear();

	std::map<std::string, C_Transform*>::iterator boneIt = boneMapping.begin();
	for (boneIt; boneIt != boneMapping.end(); ++boneIt)
	{
		std::map<std::string, Channel>::iterator channelIt = animation->channels.find(boneIt->first);

		if (channelIt != animation->channels.end())
		{
			lookUpTable[boneIt->second] = &channelIt->second;
		}
	}
}

void C_Animator::DrawBones()
{
	glColor3f(1.f, 0.f, 0.f);
	glLineWidth(4.f);
	glBegin(GL_LINES);

	//Draw lines

	bool endLine = false;
	std::map<std::string, C_Transform*>::iterator boneIt;
	for (boneIt = boneMapping.begin(); boneIt != boneMapping.end(); ++boneIt)
	{
		float3 position = boneIt->second->position;
		glVertex3f(position.x, position.y, position.z);
	}

	//LOG(LogType::L_NORMAL, "Name: %s  %f,%f,%f",bones->first.c_str(), position.x, position.y, position.z);
	glEnd();
	glLineWidth(1.f);
	glColor3f(1.f, 1.f, 1.f);
}

ResourceAnimation* C_Animator::ClipToAnimation(AnimationClip clip)
{
	ResourceAnimation* animation = new ResourceAnimation(app->resourceManager->GenerateNewUID());

	strcpy(animation->animationName, clip.name);
	animation->duration = clip.endFrame - clip.startFrame;
	animation->initTimeAnim = clip.startFrame;
	animation->loop = clip.loop;
	animation->ticksPerSecond = clip.originalAnimation->ticksPerSecond;

	return animation;
}

void C_Animator::OrderAnimation(ResourceAnimation* animation)
{
	std::vector<Channel> orderedChannels;
	std::map<std::string, C_Transform*>::iterator bone = boneMapping.begin();
	for (bone; bone != boneMapping.end(); ++bone)
	{
		if (animation->channels.find(bone->first) != animation->channels.end())
		{
			orderedChannels.push_back(animation->channels[bone->first]);
		}
	}
}

AnimationClip::AnimationClip() : name("No name"), startFrame(0), endFrame(0), originalAnimation(nullptr), loop(false) {}