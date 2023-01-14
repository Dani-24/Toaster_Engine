#include "R_Material.h"

#include "OpenGL.h"

#include "../External/ImGui/imgui.h"

#include "Application.h"

#include "ModuleScene.h"

#include "R_Shader.h"
//#include"DEJsonSupport.h"
//#include"IM_FileSystem.h"
//#include"DEJsonSupport.h"
#include "R_Texture.h"

#include "ModuleTexture.h"

ResourceMaterial::ResourceMaterial(unsigned int _uid) : Resource(_uid, Resource::Type::MATERIAL), shader(nullptr)
{}

ResourceMaterial::~ResourceMaterial()
{
	uniforms.clear();
	attributes.clear();
	shader = nullptr;
}

bool ResourceMaterial::UnloadFromMemory()
{
	SetShader(nullptr);

	//TODO: Unload resources (uniform and attributes) by reference count 
	UnloadTextures();

	uniforms.clear();
	attributes.clear();

	return true;
}

void ResourceMaterial::FillVariables()
{
	UnloadTextures();
	uniforms.clear();
	attributes.clear();

	GLint attCount = 0, uniCount = 0;
	glGetProgramiv(shader->shaderProgramID, GL_ACTIVE_ATTRIBUTES, &attCount);

	for (GLint a = 0; a < attCount; a++)
	{
		ShaderVariable shdrVar;
		glGetActiveAttrib(shader->shaderProgramID, (GLuint)a, 25, &shdrVar.nameLength, &shdrVar.vSize, &shdrVar.vType, shdrVar.name);

		attributes.push_back(shdrVar);
	}

	glGetProgramiv(shader->shaderProgramID, GL_ACTIVE_UNIFORMS, &uniCount);
	for (GLint b = 0; b < uniCount; b++)
	{
		ShaderVariable shdrVar;
		glGetActiveUniform(shader->shaderProgramID, (GLuint)b, 25, &shdrVar.nameLength, &shdrVar.vSize, &shdrVar.vType, shdrVar.name);

		uniforms.push_back(shdrVar);
	}
}

void ResourceMaterial::UnloadTextures()
{
	for (size_t i = 0; i < uniforms.size(); i++)
	{
		/*if (uniforms[i].vType == GL_SAMPLER_2D && uniforms[i].data.textureValue != nullptr)
			EngineExternal->moduleResources->UnloadResource(uniforms[i].data.textureValue->GetUID());*/
	}
}

void ResourceMaterial::PushUniforms()
{
	int used_textures = 0;
	//Push all uniforms
	for (size_t i = 0; i < uniforms.size(); ++i)
	{
		//ImGui::SameLine();
		switch (uniforms[i].vType)
		{
		case GL_SAMPLER_2D:

			glActiveTexture(GL_TEXTURE0 + used_textures);
			glUniform1i(glGetUniformLocation(shader->shaderProgramID, uniforms[i].name), used_textures);

			if (uniforms[i].data.textureValue != nullptr)
			{
				//char activeTexture[32];
				//sprintf_s(activeTexture, "activeTexture[%d]", used_textures);
				glBindTexture(GL_TEXTURE_2D, uniforms[i].data.textureValue->textureID);
				//glUniform1i(glGetUniformLocation(shader->shaderProgramID, activeTexture), true);
			}

			else if (used_textures == 0)
			{
				glBindTexture(GL_TEXTURE_2D, app->renderer3D->checkersTexture);
				//glUniform1i(glGetUniformLocation(shader->shaderProgramID, uniforms[i].name), false);
			}

			used_textures++;
			break;

		case GL_INT:
			glUniform1i(glGetUniformLocation(shader->shaderProgramID, uniforms[i].name), uniforms[i].data.intValue);
			break;

		case GL_FLOAT:
			glUniform1f(glGetUniformLocation(shader->shaderProgramID, uniforms[i].name), uniforms[i].data.floatValue);
			break;

		case GL_FLOAT_VEC2:
			glUniform2f(glGetUniformLocation(shader->shaderProgramID, uniforms[i].name), uniforms[i].data.vector2Value.x, uniforms[i].data.vector2Value.y);
			break;

		case GL_FLOAT_VEC3:
			glUniform3f(glGetUniformLocation(shader->shaderProgramID, uniforms[i].name), uniforms[i].data.vector3Value.x,
				uniforms[i].data.vector3Value.y, uniforms[i].data.vector3Value.z);
			break;

		case GL_FLOAT_VEC4:
			glUniform4f(glGetUniformLocation(shader->shaderProgramID, uniforms[i].name), uniforms[i].data.vector4Value.x,
				uniforms[i].data.vector4Value.y, uniforms[i].data.vector4Value.z, uniforms[i].data.vector4Value.w);
			break;

		default:
			break;
		}
	}
}

bool ResourceMaterial::IsDefaultUniform(const char* uniform_name)
{
	for (size_t df = 0; df < sizeof(defaultUniforms) / sizeof(defaultUniforms[0]); df++)
	{
		if (strcmp(uniform_name, defaultUniforms[df]) == 0)
		{
			return true;
		}
	}

	std::string name = uniform_name;

	for (size_t df = 0; df < sizeof(defaultUniformStructures) / sizeof(defaultUniformStructures[0]); df++)
	{
		if (name.find(defaultUniformStructures[df]) != std::string::npos)
			return true;
	}

	return false;
}

/*Set to nullptr to cleanup*/
void ResourceMaterial::SetShader(ResourceShader* res)
{
	if (shader != nullptr)
	{
		auto it = std::find(shader->references.begin(), shader->references.end(), this);
		if (shader != nullptr && it != shader->references.end())
			shader->references.erase(it);

		//Unload shader by reference count [DONE]
		/*if (shader != NULL)
			EngineExternal->moduleResources->UnloadResource(shader->GetUID());*/
	}

	shader = res;

	if (shader != nullptr)
	{
		shader->references.push_back(this);
		//Get uniforms and attributes from shader [DONE]
		FillVariables();
	}
}

ShaderVariable::ShaderVariable() : vIndex(0), vType(GL_INT), nameLength(0),
name(""), vSize(0)
{
	//data.vector3Value = float3(0, 0, 0);
	//data.intValue = 0;
}

ShaderVariable::~ShaderVariable()
{}

ShaderVariable::ShdrValue::ShdrValue() : intValue(0), floatValue(0.0f), vector2Value(0, 0), vector3Value(0, 0, 0), vector4Value(0, 0, 0, 0),
textureValue(nullptr), matrixValue(nullptr)
{}