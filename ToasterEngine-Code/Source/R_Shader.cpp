#include "R_Shader.h"

#include "OpenGL.h"
#include "Globals.h"
//#include"IM_FileSystem.h"
#include "ModuleShaderImporter.h"

#include "R_Material.h"

#include "../External/mmgr/mmgr.h"
//#include "../External/ImGui/imgui.h"

ResourceShader::ResourceShader(unsigned int _uid) : Resource(_uid, Resource::Type::SHADER), shaderProgramID(0)
{
	for (GLuint i = 0; i < static_cast<GLuint>(ShaderType::SH_Max); i++)
		shaderObjects[i] = 0;
}

ResourceShader::~ResourceShader()
{
	references.clear();
	if (shaderProgramID != 0)
	{
		glDeleteShader(shaderProgramID);
		shaderProgramID = 0;
	}
}

void ResourceShader::LinkToProgram()
{
	shaderProgramID = glCreateProgram();

	for (size_t i = 0; i < static_cast<int>(ShaderType::SH_Max); i++)
	{
		if (shaderObjects[i] != 0)
			glAttachShader(shaderProgramID, shaderObjects[i]);
	}

	glLinkProgram(shaderProgramID);

	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
		LOG("Error linking shader program: %s", infoLog);
	}

	for (size_t i = 0; i < static_cast<int>(ShaderType::SH_Max); i++)
	{
		glDetachShader(shaderProgramID, shaderObjects[i]);
	}

	for (size_t i = 0; i < static_cast<int>(ShaderType::SH_Max); i++)
	{
		glDeleteShader(shaderObjects[i]);
		shaderObjects[i] = 0;
	}

	for (std::vector<ResourceMaterial*>::iterator i = references.begin(); i != references.end(); ++i)
	{
		(*i)->FillVariables();
	}
}

bool ResourceShader::UnloadFromMemory()
{
	for (size_t i = 0; i < static_cast<size_t>(ShaderType::SH_Max); i++)
	{
		glDeleteShader(shaderObjects[i]);
		shaderObjects[i] = 0;
	}

	glDeleteShader(shaderProgramID);
	shaderProgramID = 0;
	return true;
}

void ResourceShader::Bind()
{
	glUseProgram(shaderProgramID);
}

void ResourceShader::Unbind()
{
	glUseProgram(0);
}