#pragma once

#include "Resource.h"
#include <vector>

typedef unsigned int GLuint;
class ResourceMaterial;

enum class ShaderType
{
	SH_Vertex,
	SH_Frag,
	SH_Geometry,
	SH_Max
};

class ResourceShader : public Resource
{

public:
	ResourceShader(unsigned int _uid);
	~ResourceShader();

	void LinkToProgram();

	bool UnloadFromMemory() override;

	void Bind();
	void Unbind();

	GLuint shaderObjects[static_cast<int>(ShaderType::SH_Max)];

	GLuint shaderProgramID;

	std::vector<ResourceMaterial*> references;
};