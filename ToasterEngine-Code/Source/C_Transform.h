#pragma once
#include "Component.h"

#include "../External/MathGeoLib/include/Math/float4x4.h"
#include "../External/MathGeoLib/include/Math/float3.h"
#include "../External/MathGeoLib/include/Math/Quat.h"

#include <vector>

//class Command
//{
//public:
//	Command(int agentUid);
//	virtual ~Command();
//
//	virtual void Execute() = 0;
//	virtual void Undo() = 0;
//
//protected:
//	int agentUid;
//};
//
//class COMM_Transform : public Command
//{
//public:
//	COMM_Transform(int agentUid, float* nextMatrix, float* previousMatrix);
//	~COMM_Transform() override;
//
//	void Execute() override;
//	void Undo() override;
//
//private:
//	float nextMatrix[16];
//	float previousMatrix[16];
//};

class GameObject;

class C_Transform : public Component
{
public:

	C_Transform();
	C_Transform(GameObject* _gm/*, float3 _position, Quat _rotation, float3 _localScale*/);
	virtual ~C_Transform();

	void PostUpdate() override;

	bool OnEditor() override;

	float4x4 GetCurrentGlobalMatrix();

	float4x4 globalTransform;
	float4x4 localTransform;

	float3 position, localScale, eulerRotation;
	Quat rotation;

	bool updateTransform, sendCommand;
	void SetTransformMatrix(float3 _position, Quat _rotation, float3 _localScale);

	void SetTransformWithGlobal(float4x4& globalMat);

	const float* GetGlobalTransposed() const;
	void ResetTransform();

	float3 GetForward();
	float3 GetUp();
	float3 GetRight();
	void UpdateBoxes();
	void LookAt(float3 pointToLook);

private:
	void UpdateTransform();
	C_Transform* GetRecursiveTransforms(C_Transform* node, std::vector<C_Transform*>& transforms);


	float4x4 globalTransformTRANS;
	float4x4 oldTransform;

	float3 GetNormalizeAxis(int i);
};