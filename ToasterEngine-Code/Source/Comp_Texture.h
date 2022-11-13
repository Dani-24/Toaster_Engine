#pragma once

#include "Component.h"

#include "ModuleTexture.h"

#include "Globals.h"

class Comp_Texture : public Component {
public:
	Comp_Texture(GameObject* gameObject, uint t);
	~Comp_Texture() override;

	void AddTexture(uint t);

	uint GetTexture() override;

	void OnEditor();

private:
	uint texture;

	uint originalTexture;

	bool renderTexture = true;
	bool deleteTexture = false;
};