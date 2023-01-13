#include "R_Mesh.h"

#include "OpenGL.h"
//#include "MeshArrays.h"
//#include "IM_FileSystem.h"
#include "Globals.h"

#include "R_Shader.h"
#include "R_Material.h"
#include "R_Texture.h"

#include"Application.h"

#include "ModuleRenderer3D.h"

#include "GameObject.h"
#include "C_Transform.h"

#include "DtTimer.h"

ResourceMesh::ResourceMesh(unsigned int _uid) : Resource(_uid, Resource::Type::MESH), indices_id(0), vertices_id(0),
EBO(0), VAO(0), VBO(0)
{
}

ResourceMesh::~ResourceMesh()
{
	boneTransforms.clear();
	bonesMap.clear();
	bonesOffsets.clear();
}

bool ResourceMesh::LoadToMemory()
{
	LOG("Mesh loaded to memory");

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, (GLuint*)&(VBO));
	glGenBuffers(1, (GLuint*)&(EBO));

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * VERTEX_ATTRIBUTES * vertices_count, vertices, GL_STATIC_DRAW);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices_count, indices, GL_STATIC_DRAW);

	//position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)0);

	//texcoords attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(TEXCOORD_OFFSET * sizeof(GLfloat)));

	//normals attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(NORMALS_OFFSET * sizeof(GLfloat)));

	//tangents attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(TANGENTS_OFFSET * sizeof(GLfloat)));

	//joint indices
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(BONES_ID_OFFSET * sizeof(GLfloat)));

	//weights
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(WEIGHTS_OFFSET * sizeof(GLfloat)));

	//colors
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(COLORS_OFFSET * sizeof(GLfloat)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

bool ResourceMesh::UnloadFromMemory()
{
	glDeleteVertexArrays(1, &VAO);
	VAO = 0u;
	glDeleteBuffers(1, &VBO);
	VBO = 0u;
	glDeleteBuffers(1, &EBO);
	EBO = 0u;

	//Clear buffers
	if (indices != nullptr)
		delete[] indices;

	if (vertices != nullptr)
		delete[] vertices;

	//Restart to nullptr
	indices = nullptr;
	vertices = nullptr;

	return true;
}

void ResourceMesh::RenderMesh(GLuint textureID, float3 color, bool renderTexture, ResourceMaterial* material, C_Transform* _transform,
	ResourceTexture* normalMap, ResourceTexture* specularMap, float bumpDepth, float emissionAmmount)
{

	if (material->shader)
	{
		material->shader->Bind();
		material->PushUniforms();

		PushDefaultMeshUniforms(material->shader->shaderProgramID, textureID, _transform, color, normalMap, specularMap, bumpDepth, emissionAmmount);

		//EngineExternal->moduleRenderer3D->PushLightUniforms(material);
	}

	//vertices
	OGL_GPU_Render();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (material->shader)
		material->shader->Unbind();
}

void ResourceMesh::OGL_GPU_Render()
{
	//vertices
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}

void ResourceMesh::RenderMeshDebug(bool* vertexNormals, bool* faceNormals, const float* globalTransform)
{
	glPushMatrix();
	glMultMatrixf(globalTransform);

	if (*vertexNormals == true)
	{
		float normalLenght = 0.25f;
		glPointSize(3.0f);
		glColor3f(1, 0, 0);
		glBegin(GL_POINTS);
		for (unsigned int i = 0; i < vertices_count; i++)
		{
			glVertex3f(vertices[i * VERTEX_ATTRIBUTES], vertices[i * VERTEX_ATTRIBUTES + 1], vertices[i * VERTEX_ATTRIBUTES + 2]);
		}
		glEnd();
		glColor3f(0, 1, 0);
		glPointSize(1.0f);

		//Vertex normals
		glColor3f(0, 1, 0);
		glBegin(GL_LINES);
		for (unsigned int i = 0; i < vertices_count; i++)
		{
			glVertex3f(vertices[i * VERTEX_ATTRIBUTES], vertices[i * VERTEX_ATTRIBUTES + 1], vertices[i * VERTEX_ATTRIBUTES + 2]);
			glVertex3f(vertices[i * VERTEX_ATTRIBUTES] + vertices[i * VERTEX_ATTRIBUTES + 5] * normalLenght,
				vertices[i * VERTEX_ATTRIBUTES + 1] + vertices[i * VERTEX_ATTRIBUTES + 6] * normalLenght,
				vertices[i * VERTEX_ATTRIBUTES + 2] + vertices[i * VERTEX_ATTRIBUTES + 7] * normalLenght);
		}
		glEnd();
		glColor3f(1, 1, 1);
	}

	if (*faceNormals == true)
	{
		float normalLenght = 0.25f;
		//Face normals
		glColor3f(1, 0, 0);
		glBegin(GL_LINES);
		for (int i = 0; i < indices_count; i += 3)
		{
			vec3 A = GetVectorFromIndex(&vertices[indices[i] * VERTEX_ATTRIBUTES]);
			vec3 B = GetVectorFromIndex(&vertices[indices[i + 1] * VERTEX_ATTRIBUTES]);
			vec3 C = GetVectorFromIndex(&vertices[indices[i + 2] * VERTEX_ATTRIBUTES]);

			vec3 middle((A.x + B.x + C.x) / 3.f, (A.y + B.y + C.y) / 3.f, (A.z + B.z + C.z) / 3.f);

			vec3 crossVec = cross((B - A), (C - A));
			vec3 normalDirection = normalize(crossVec);

			glVertex3f(middle.x, middle.y, middle.z);
			glVertex3f(middle.x + normalDirection.x * normalLenght, middle.y + normalDirection.y * normalLenght, middle.z + normalDirection.z * normalLenght);
		}
		glEnd();
		glPointSize(1.f);
		glColor3f(1, 1, 1);
	}

	glPopMatrix();
}

vec3 ResourceMesh::GetVectorFromIndex(float* startValue)
{
	float x = *startValue;
	++startValue;
	float y = *startValue;
	++startValue;
	float z = *startValue;

	return vec3(x, y, z);
}

void ResourceMesh::GetVertices(float* verticesArray)
{
	for (size_t i = 0; i < vertices_count; i++)
	{
		verticesArray[i * 3] = vertices[i * VERTEX_ATTRIBUTES];
		verticesArray[i * 3 + 1] = vertices[i * VERTEX_ATTRIBUTES + 1];
		verticesArray[i * 3 + 2] = vertices[i * VERTEX_ATTRIBUTES + 2];
	}
}

int* ResourceMesh::GetTriangles() const
{
	return (int*)(indices);
}

bool ResourceMesh::HasVertexColors()
{
	return vertices[COLORS_OFFSET] != 0.0f && vertices[COLORS_OFFSET + 1] != 0.0f && vertices[COLORS_OFFSET + 2] != 0.0f;
}

void ResourceMesh::SaveBones(char** cursor)
{
	uint bytes = 0;

	if (bonesOffsets.size() > 0)
	{
		for (int i = 0; i < bonesOffsets.size(); ++i)
		{
			bytes = sizeof(float) * 16;
			memcpy(*cursor, bonesOffsets[i].ptr(), bytes);
			*cursor += bytes;
		}
	}

	for (int i = 0; i < bonesMap.size(); ++i)
	{
		std::map<std::string, uint>::const_iterator it;
		for (it = bonesMap.begin(); it != bonesMap.end(); ++it)
		{
			if (it->second == i)
			{
				bytes = sizeof(uint);
				uint stringSize = it->first.size();
				memcpy(*cursor, &stringSize, bytes);
				*cursor += bytes;

				bytes = sizeof(char) * stringSize;
				memcpy(*cursor, it->first.c_str(), bytes);
				*cursor += bytes;
			}
		}
	}
}

void ResourceMesh::LoadBones(char** cursor)
{
	uint bytes = 0;

	//Fills the mesh boneOffset matrix by copying on an empty of 4x4 size and setting later
	float offsets_matrix[16];
	for (uint i = 0; i < bonesOffsets.size(); ++i)
	{
		bytes = sizeof(float) * 16;
		memcpy(offsets_matrix, *cursor, bytes);
		*cursor += bytes;

		float4x4 offset;
		offset.Set(offsets_matrix);
		bonesOffsets[i] = offset;
	}

	//Fills the mesh boneMap by getting the string size and using it to read the name stored and setting it on the map
	char name[30];
	for (uint i = 0; i < boneTransforms.size(); ++i)
	{
		bytes = sizeof(uint);
		uint nameSize = 0;
		memcpy(&nameSize, *cursor, bytes);
		*cursor += bytes;

		bytes = sizeof(char) * nameSize;
		memcpy(name, *cursor, bytes);
		*cursor += bytes;

		name[nameSize] = '\0';
		std::string str(name);
		bonesMap[str.c_str()] = i;
	}
}

void ResourceMesh::PushDefaultMeshUniforms(uint shaderID, uint textureID, C_Transform* _transform, float3 color, ResourceTexture* normalMap, ResourceTexture* specularMap, float bumpDepth, float emissionAmmount)
{
	if (textureID != 0)
		glUniform1i(glGetUniformLocation(shaderID, "hasTexture"), 1);
	else
		glUniform1i(glGetUniformLocation(shaderID, "hasTexture"), 0);

	GLint modelLoc = glGetUniformLocation(shaderID, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, _transform->GetGlobalTransposed());

	if (app->renderer3D->renderOnThisCamera != nullptr)
	{
		modelLoc = glGetUniformLocation(shaderID, "view");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, app->renderer3D->renderOnThisCamera->ViewMatrixOpenGL().ptr());

		modelLoc = glGetUniformLocation(shaderID, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, app->renderer3D->renderOnThisCamera->ProjectionMatrixOpenGL().ptr());

		modelLoc = glGetUniformLocation(shaderID, "cameraPosition");
		float3 cameraPosition = app->renderer3D->renderOnThisCamera->GetPosition();
		glUniform3fv(modelLoc, 1, &cameraPosition.x);
	}

	modelLoc = glGetUniformLocation(shaderID, "normalMatrix");
	float3x3 normalMatrix = _transform->globalTransform.Float3x3Part().Transposed();
	normalMatrix.Inverse();
	normalMatrix.Transpose();
	glUniformMatrix3fv(modelLoc, 1, GL_FALSE, normalMatrix.ptr());

	modelLoc = glGetUniformLocation(shaderID, "time");
	glUniform1f(modelLoc, DETime::realTimeSinceStartup);

	modelLoc = glGetUniformLocation(shaderID, "altColor");
	glUniform3fv(modelLoc, 1, &color.x);

	glActiveTexture(GL_TEXTURE15);
	glUniform1i(glGetUniformLocation(shaderID, "normalMap"), 15);

	if (normalMap != nullptr)
		glBindTexture(GL_TEXTURE_2D, normalMap->textureID);

	else
		glBindTexture(GL_TEXTURE_2D, app->renderer3D->defaultNormalMap);

	glActiveTexture(GL_TEXTURE16);
	glUniform1i(glGetUniformLocation(shaderID, "specularMap"), 16);

	if (specularMap != nullptr)
		glBindTexture(GL_TEXTURE_2D, specularMap->textureID);

	else
		glBindTexture(GL_TEXTURE_2D, app->renderer3D->defaultSpecularMap);

	modelLoc = glGetUniformLocation(shaderID, "bumpDepth");
	glUniform1f(modelLoc, bumpDepth);


	if (boneTransforms.size() > 0)
	{
		modelLoc = glGetUniformLocation(shaderID, "jointTransforms");
		glUniformMatrix4fv(modelLoc, boneTransforms.size(), GL_FALSE, (GLfloat*)&boneTransforms[0]);

		modelLoc = glGetUniformLocation(shaderID, "jointTransforms");
		glUniformMatrix4fv(modelLoc, boneTransforms.size(), GL_FALSE, (GLfloat*)&boneTransforms[0]);

		modelLoc = glGetUniformLocation(shaderID, "hasAnimations");
		glUniform1i(modelLoc, 1);
	}
	else
	{
		modelLoc = glGetUniformLocation(shaderID, "hasAnimations");
		glUniform1i(modelLoc, 0);
	}

	modelLoc = glGetUniformLocation(shaderID, "emissiveAmmount");
	glUniform1f(modelLoc, emissionAmmount);
}