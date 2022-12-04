#include "Globals.h"
#include "Application.h"
#include "ModuleMesh3D.h"

#pragma comment (lib, "External/Assimp/libx86/assimp.lib")

#include "OpenGL.h"

#pragma comment( lib, "External/DevIL/libx86/DevIL.lib" )
#pragma comment( lib, "External/DevIL/libx86/ILU.lib" )
#pragma comment( lib, "External/DevIL/libx86/ILUT.lib" )

#include "../External/DevIL/include/ilu.h"
#include "../External/DevIL/include/ilut.h"
#include "../External/DevIL/include/il.h"

#include "GameObject.h"

ModuleMesh3D::ModuleMesh3D(Application* app, bool start_enabled) : Module(app, start_enabled){}
ModuleMesh3D::~ModuleMesh3D(){}

bool ModuleMesh3D::Start()
{
	LOG("TOASTER: Setting up mesh loader");
	bool ret = true;

	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	ilInit();
	iluInit();
	ilutInit();

	return ret;
}

bool ModuleMesh3D::CleanUp()
{
	//LOG("TOASTER: Cleaning meshes");

	/*for (int i = 0; i < meshes.size(); i++) {
		delete meshes[i];
		meshes[i] = nullptr;
	}
	meshes.clear();*/

	// detach log stream
	aiDetachAllLogStreams();

	return true;
}

update_status ModuleMesh3D::PostUpdate(float dt)
{
	// Check Wireframe options
	if (app->renderer3D->wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return UPDATE_CONTINUE;
}

Mesh* ModuleMesh3D::LoadFile(string file_path, GameObject* go)
{
	const aiScene* scene = aiImportFile(file_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

	std::vector<Mesh*> meshes;

	/*Mesh* mesh = new Mesh();
	mesh->path = file_path;*/

	if (scene != nullptr && scene->HasMeshes())
	{
		for (int i = 0; i < scene->mNumMeshes; i++) {
			aiMesh* sceneMesh = new aiMesh();
			sceneMesh = scene->mMeshes[i];

			Mesh* meshData = new Mesh();
			meshData->path = file_path;

			Import(sceneMesh, meshData);
			meshes.push_back(meshData);
		}

		//Iterate scene meshes
		/*for (int i = 0; i < scene->mNumMeshes; i++) {

			//Copy fbx mesh info to Mesh struct
			mesh->num_vertices = scene->mMeshes[i]->mNumVertices;
			mesh->vertices = new float[mesh->num_vertices * VERTEX_ARG];

			for (int v = 0; v < mesh->num_vertices; v++) {
				//vertices
				mesh->vertices[v * VERTEX_ARG] = scene->mMeshes[i]->mVertices[v].x;
				mesh->vertices[v * VERTEX_ARG + 1] = scene->mMeshes[i]->mVertices[v].y;
				mesh->vertices[v * VERTEX_ARG + 2] = scene->mMeshes[i]->mVertices[v].z;

				//uvs
				if (scene->mMeshes[i]->HasTextureCoords(0)) {
					mesh->vertices[v * VERTEX_ARG + 3] = scene->mMeshes[i]->mTextureCoords[0][v].x;
					mesh->vertices[v * VERTEX_ARG + 4] = scene->mMeshes[i]->mTextureCoords[0][v].y;
				}
				else {
					mesh->vertices[v * VERTEX_ARG + 3] = 0;
					mesh->vertices[v * VERTEX_ARG + 4] = 0;
				}
			}

			LOG("Loading 3d mesh %s with %d vertices", file_path.c_str(), mesh->num_vertices);

			//Load Faces
			if (scene->mMeshes[i]->HasFaces())
			{
				//Copy fbx mesh indices info to Mesh struct
				mesh->num_indices = scene->mMeshes[i]->mNumFaces * 3;
				mesh->indices = new uint[mesh->num_indices]; // assume each face is a triangle

				//Iterate mesh faces
				for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
				{
					// TRIANGLE SUPREMACY
					if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3) {
						LOG("Warning, do not enter this game if you're under 18 years old- geometry face with != 3 indices!");
					}
					else {
						memcpy(&mesh->indices[j * 3], scene->mMeshes[i]->mFaces[j].mIndices, 3 * sizeof(uint));
					}
				}

				//Add mesh to array
				LoadMesh(mesh);

				meshes.push_back(mesh);
			}
			else {
				LOG("This toast has no faces: %s", file_path.c_str());
				delete mesh;
			}
		}*/
		aiReleaseImport(scene);
	}
	else {
		LOG("This has no meshes or it's cursed -> %s", file_path.c_str());
	}

	if (meshes.size() < 2) {
		return meshes[0];
	}
	else {
		for (int i = 0; i < meshes.size(); i++) {
			if (go == nullptr) {
				go = app->editor->root;
			}
			GameObject* meshChild = new GameObject(file_path.c_str(), go);
			meshChild->AddTexture(go->GetTexture());
			meshChild->AddMesh(meshes[i]);
		}
		go->DeleteTextures();
		return nullptr;
	}
}

void ModuleMesh3D::LoadMesh(Mesh* mesh)
{
	glEnableClientState(GL_VERTEX_ARRAY);

	//Create vertices and indices buffers
	glGenBuffers(1, (GLuint*)&(mesh->id_vertices));
	glGenBuffers(1, (GLuint*)&(mesh->id_indices));
	glGenBuffers(1, (GLuint*)&(mesh->id_normals));
	glGenBuffers(1, (GLuint*)&(mesh->id_textureCoords));

	//Bind and fill buffers
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertices * VERTEX_ARG, mesh->vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->num_indices, mesh->indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_normals * 3, mesh->normals, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_textureCoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_textureCoords * 2, mesh->textureCoords, GL_STATIC_DRAW);

	//Unbind buffers
	glDisableClientState(GL_VERTEX_ARRAY);

}

void ModuleMesh3D::Import(const aiMesh* sceneMesh, Mesh* meshData) {
	//copy vertices
	meshData->num_vertices = sceneMesh->mNumVertices;
	meshData->vertices = new float[meshData->num_vertices * VERTEX_ARG];

	memcpy(meshData->vertices, sceneMesh->mVertices, sizeof(float) * meshData->num_vertices * VERTEX_ARG);

	LOG("New mesh with %d vertices", meshData->num_vertices);

	//copy faces
	if (sceneMesh->HasFaces())
	{
		meshData->num_indices = sceneMesh->mNumFaces * 3;
		meshData->indices = new uint[meshData->num_indices]; // assume each face is a triangle
		for (uint i = 0; i < sceneMesh->mNumFaces; ++i)
		{
			if (sceneMesh->mFaces[i].mNumIndices != 3) {
				LOG("WARNING, geometry face with != 3 indices!");
			}
			else
			{
				memcpy(&meshData->indices[i * 3], sceneMesh->mFaces[i].mIndices, 3 * sizeof(uint));
			}
		}
	}

	//copy normals
	if (sceneMesh->HasNormals()) {
		meshData->num_normals = sceneMesh->mNumVertices;
		meshData->normals = new float[meshData->num_normals * 3];

		memcpy(meshData->normals, sceneMesh->mNormals, sizeof(float) * meshData->num_normals * 3);
	}

	//copy texture coords
	if (sceneMesh->HasTextureCoords(0)) {
		meshData->num_textureCoords = sceneMesh->mNumVertices;
		meshData->textureCoords = new float[sceneMesh->mNumVertices * 2];

		for (unsigned int i = 0; i < meshData->num_textureCoords; i++) {
			meshData->textureCoords[i * 2] = sceneMesh->mTextureCoords[0][i].x;
			meshData->textureCoords[i * 2 + 1] = sceneMesh->mTextureCoords[0][i].y;
		}
	}

	LoadMesh(meshData);
}

// === MESH === 
Mesh::~Mesh() {
	delete[] vertices;
	delete[] indices;
	delete[] normals;
	delete[] textureCoords;
	vertices = nullptr;
	indices = nullptr;
	normals = nullptr;
	textureCoords = nullptr;
	glDeleteBuffers(1, &id_vertices);
	glDeleteBuffers(1, &id_indices);
	glDeleteBuffers(1, &id_normals);
	glDeleteBuffers(1, &id_textureCoords);
	id_vertices = 0;
	id_indices = 0;
	id_normals = 0;
	id_textureCoords = 0;
}

void Mesh::Render(uint texture, mat4x4 matrix)
{
	if (texture != NULL) {
		glBindTexture(GL_TEXTURE_2D, texture);
	}
		
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);

	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, id_normals);
	glNormalPointer(GL_FLOAT, 0, NULL);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, id_textureCoords);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glPushMatrix();
	glMultMatrixf(&matrix);

	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}