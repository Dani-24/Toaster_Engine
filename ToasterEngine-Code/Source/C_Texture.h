#pragma once

#include "Component.h"

class ResourceTexture;
class ResourceMaterial;

class C_Material : public Component
{
public:
	C_Material(GameObject* _gm);
	virtual ~C_Material();

	bool OnEditor() override;

	void SetMaterial(ResourceMaterial* newMaterial);
	int GetTextureID();

	bool viewWithCheckers;

	ResourceTexture* matTexture;
	ResourceMaterial* material;
};