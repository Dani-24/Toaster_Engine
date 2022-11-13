#pragma once

#include "Component.h"

struct Transform
{
	float3 position = float3(0.0f, 0.0f, 0.0f);
	float3 scale = float3(0.0f, 0.0f, 0.0f);
	float3 rotation = float3(0.0f, 0.0f, 0.0f);
};

class Comp_Transform : public Component {
public:
	Comp_Transform(GameObject* go);
	~Comp_Transform() {};

	void SetPos(float3 pos);
	void SetRot(float3 rot);
	void SetScale(float3 scale);

	float3 GetPos() { return componentTrans.position; }
	float3 GetRot() { return componentTrans.rotation; }
	float3 GetScale() { return componentTrans.scale; }

	void SetTransform(float3 pos, float3 rot, float3 scale);

	void Translate(float3 translation);
	void Rotate(float3 rot);
	void Scale(float3 scale);

	void UpdatePosition();
	void UpdateRotation();
	void UpdateScale();
	void UpdateTransform();

	void ParentPositionUpdate(float3 pos) override;
	void ParentScaleUpdate(float3 scale) override;
	void ParentRotationUpdate(float3 rot) override;
	void ParentTransformUpdate(float3 pos, float3 rot, float3 scale) override;

	void SetTransformMatrix(float3 _position, float3 _rotation, float3 _scale);

	Transform GetGlobalTransform();

	void OnEditor();

private:
	
	Transform componentTrans;
	Transform componentParentTrans;

	float4x4 goTransform;
};