#pragma once

#include "Globals.h"
#include "../External/MathGeoLib/include/MathGeoLib.h"
#include "../External/ImGui/imgui.h"

class GameObject;

class Component {
public:
	enum class Comp_Type {
		Transform,
		Mesh,
		Texture,
		ToastyAbsolutelyNothing,
	};

	Component(GameObject* go, Comp_Type ctype);
	virtual ~Component();

	Comp_Type GetCompType() { return ctype; }

	GameObject* GetGameObject() { return go; }

private:	
	Comp_Type ctype = Comp_Type::ToastyAbsolutelyNothing;

protected:
	GameObject* go = nullptr;

public:
	virtual void ParentPositionUpdate(float3 pos){}
	virtual void ParentScaleUpdate(float3 scale){}
	virtual void ParentRotationUpdate(float3 rot){}
	virtual void ParentTransformUpdate(float3 pos, float3 rot, float3 scale){}

	virtual void RenderMesh(){}
	virtual uint GetTexture() { return 0; }

	virtual void OnEditor() {};
};