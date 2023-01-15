#pragma once

#include "Globals.h"

#include "ModuleMesh3D.h"
#include "ModuleTexture.h"
#include "ModuleCamera3D.h"
#include "ModuleAnimation.h"

#include <math.h>
#include "../External/ImGui/imgui.h"

struct Transform {
	vec3 position = vec3(0.0f, 0.0f, 0.0f),
		rotation = vec3(0.0f, 0.0f, 0.0f),
		scale = vec3(1.0f, 1.0f, 1.0f);
};

struct GameObject;

struct AnimationClip
{
	AnimationClip();
	char name[32];
	float startFrame, endFrame;
	bool loop;
	Animation* originalAnimation;
};

struct GOTransC {
	GameObject* go;
	float original = 0;
	vec3 movement;
};

struct TransAnimationClip {

	GOTransC moaiMov, cubeBodyMov, cubeLeftArmMov, cubeRightArmMov, cubeRightLegMov, cubeLeftLegMov;

	string name;
	float startFrame = 0, endFrame, midFrame, currentFrame = 0;
	bool loop = false;
};

class GameObject
{
public:
	GameObject(std::string name, GameObject* parent, Camera* cam = nullptr);
	~GameObject();

	void DeleteThisGameObject();

	// Hierarchy
private:
	void SetParent(GameObject* parent);

public:
	GameObject* GetParent() { return parent; }

	void AddChild(GameObject* child);
	void DeleteChild(GameObject* child);

	// Vars
	std::string GetName() { return name; }
	uint GetID() { return ID; }

	// ImGUI
	void OnEditor();

private:

	uint ID;
	GameObject* parent = nullptr;

public:

	std::string name;
	std::vector<GameObject*> childs;

	bool pendindToDelete = false;

	// TRANSFORM
public:

	void SetPos(vec3 pos);
	void SetRot(vec3 rot);
	void SetScale(vec3 scale);

	vec3 GetPos() { return GO_trans.position; }
	vec3 GetRot() { return GO_trans.rotation; }
	vec3 GetScale() { return GO_trans.scale; }

	void SetTransform(vec3 pos, vec3 rot, vec3 scale);

	// Apply Transformations
	void UpdatePosition();
	void UpdateRotation();
	void UpdateScale();
	void UpdateTransform();

	// Matrix
	void SetTransformMatrix(vec3 _position, vec3 _rotation, vec3 _scale);
	void SetGlobalMatrix();
	Transform GetGlobalTransform();

	// Parent
	void ParentPosUpdate(vec3 pos);
	void ParentRotUpdate(vec3 rot);
	void ParentScaleUpdate(vec3 scale);
	void ParentTransUpdate(vec3 pos, vec3 rot, vec3 scale);

public:
	mat4x4 GO_matrix;

	Transform GO_trans;
private:
	Transform GO_parentTrans, originalParentTrans;

public:
	// MESH
	void AddMesh(Mesh* m);

	void RenderMesh();
	void ShowMesh(bool visible) { renderMesh = visible; }

	Mesh* GetMesh() { return GO_mesh; }

	void DisplayMesh(bool display);

private:

	Mesh* GO_mesh = nullptr;

public:
	bool renderMesh;

	bool alwaysRender = false;

public:
	// TEXTURE
	void AddTexture(Texture* texture);
	Texture* GetTexture() { return GO_texture; }
	void DeleteTextures();

private:

	std::vector<Texture*> GO_allTextures;
	Texture* GO_texture = nullptr;

	bool renderTexture;

	// Camera
public:
	Camera* GO_camera;

	float3 camLookAt = float3(0.0f, 1.0f, 0.0f);

	// AABB
public:
	void CreateAABB();
	void DrawAABB();

	AABB aabb;

	// ANIMATIONS
public:

	bool playingAnAnimation = true;
	bool started = false;
	float time = 0.f;
	float speed = 20.f;

	bool channelsLinked = false;
	bool bonesLinked = false;
	bool drawBones = false;

	// Blending
	float blendTime = 0.f;
	float blendTimeDuration = 0.f;

	uint prevAnimationT = 0;
	uint currentAnimationT = 0;

	Animation* prevAnimation = nullptr;
	Animation* currentAnimation = nullptr;

	// Load Animations
	void AddAnimation(Animation* animation);
	void AddAnimation(std::vector<Animation*> animations);

	std::vector<Animation*> GO_animations;

	void AddClip(Animation* anim);

	std::vector<AnimationClip> clips;
	AnimationClip* selectedClip = nullptr;

	Animation* ClipToAnim(AnimationClip clip);

	// Bones
	GameObject* rootBone = nullptr;
	std::map<std::string, GameObject*> bones;

	uint rootBoneID;

	Channel*  bone;

	std::map <GameObject*, Channel*> bonesCurrentAnim;
	std::map <GameObject*, Channel*> bonesPrevAnim;

	void DrawBones(GameObject* p);
	void StoreBoneMapping(GameObject* go);
	bool FindRootBone();

	// Channels
	void UpdateChannelsTransform(const Animation* settings, const Animation* blend, float blendRatio);

	float3	GetChannelPosition(const Channel& ch, float currentKey, float3 defPos) const;
	float3	GetChannelRotation(const Channel& ch, float currentKey, float3 defRot) const;
	float3	GetChannelScale(const Channel& ch, float currentKey, float3 defScale) const;

	void StartAnimation();
	void UpdateAnimation(float dt);

	void DeleteAnimation(Animation* anim);

	void PlayAnim(Animation* anim, float blendDuration = 0.2f, float Speed = 1.0f);
	void PauseAnim();
	void ResumeAnim();

	void SetAnimationChannelToBones(Animation* animation, std::map<GameObject*, Channel*>& lookUpTable);

	// Transform ANimations
	public:
	bool animatedTransform = false;

	std::vector<TransAnimationClip*> transClips;
	TransAnimationClip* currentTransClip = nullptr, *previousTransClip = nullptr;

	void PlayAnim(TransAnimationClip* anim, float blendDuration = 0.2f, float Speed = 1.0f);

	void UpdateTransAnim(float dt);

	void AddTransAnimation(TransAnimationClip* anim);

	void AnimateTrans(float speed, bool positive);

	void Idle();
	void Walk();
	void Kick();
};