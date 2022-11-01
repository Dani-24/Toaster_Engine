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

ModuleMesh3D::ModuleMesh3D(Application* app, bool start_enabled) : Module(app, start_enabled){}
ModuleMesh3D::~ModuleMesh3D(){}

// -----------------------------------------------------------------
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

// -----------------------------------------------------------------
bool ModuleMesh3D::CleanUp()
{
	LOG("TOASTER: Cleaning meshes");

	for (int i = 0; i < meshes.size(); i++) {
		delete meshes[i];
		meshes[i] = nullptr;
	}
	meshes.clear();

	// detach log stream
	aiDetachAllLogStreams();

	return true;
}

// -----------------------------------------------------------------
update_status ModuleMesh3D::PostUpdate(float dt)
{
	// Check Wireframe options
	if (app->renderer3D->wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// DRAW
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i]->Render();
	}  

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return UPDATE_CONTINUE;
}

Mesh* ModuleMesh3D::LoadFile(string file_path)
{
	const aiScene* scene = aiImportFile(file_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

	Mesh* mesh = new Mesh();
	if (scene != nullptr && scene->HasMeshes())
	{
		//Iterate scene meshes
		for (int i = 0; i < scene->mNumMeshes; i++) {
			
			//Copy fbx mesh info to Mesh struct
			mesh->num_vertices = scene->mMeshes[i]->mNumVertices;
			mesh->vertices = new float[mesh->num_vertices * VERTEX_ARG];

			for (int v = 0; v < mesh->num_vertices; v++) {
				//vertices
				mesh->vertices[v * VERTEX_ARG] = scene->mMeshes[i]->mVertices[v].x;
				mesh->vertices[v * VERTEX_ARG + 1] = scene->mMeshes[i]->mVertices[v].y;
				mesh->vertices[v * VERTEX_ARG + 2] = scene->mMeshes[i]->mVertices[v].z;

				//uvs
				mesh->vertices[v * VERTEX_ARG + 3] = scene->mMeshes[i]->mTextureCoords[0][v].x;
				mesh->vertices[v * VERTEX_ARG + 4] = scene->mMeshes[i]->mTextureCoords[0][v].y;
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
			}
			else {
				LOG("This toast has no faces: %s", file_path.c_str());
				delete mesh;
			}
		}
		aiReleaseImport(scene);
	}
	else {
		LOG("Error loading this shit %s", file_path.c_str());
		delete mesh;
	}
	return mesh;
}

void ModuleMesh3D::LoadMesh(Mesh* mesh)
{
	glEnableClientState(GL_VERTEX_ARRAY);

	//Create vertices and indices buffers
	glGenBuffers(1, (GLuint*)&(mesh->id_vertices));
	glGenBuffers(1, (GLuint*)&(mesh->id_indices));

	//Bind and fill buffers
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertices * VERTEX_ARG, mesh->vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->num_indices, mesh->indices, GL_STATIC_DRAW);

	//Unbind buffers
	glDisableClientState(GL_VERTEX_ARRAY);

	//Add mesh to meshes vector
	meshes.push_back(mesh);
}

// === MESH === 
Mesh::~Mesh() {
	delete[] vertices;
	delete[] indices;
	vertices = nullptr;
	indices = nullptr;
	glDeleteBuffers(1, &id_vertices);
	glDeleteBuffers(1, &id_indices);
	id_vertices = 0;
	id_indices = 0;
}

void Mesh::Render()
{
	glEnable(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	//glBindTexture(GL_TEXTURE_2D, /*textIDs*/);

	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);

	// Array vertex
	glVertexPointer(3, GL_FLOAT, sizeof(float) * VERTEX_ARG, NULL);
	glTexCoordPointer(2, GL_FLOAT, sizeof(float) * VERTEX_ARG, (void*)(3 * sizeof(float)));

	// Draw
	glPushMatrix(); 
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);
	glPopMatrix();

	// Unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_COORD_ARRAY);
}

uint ModuleMesh3D::LoadTexture(string path) {

	ILubyte* Lump;
	ILuint Size;
	FILE* File;

	File = fopen(path.c_str(), "rb");
	fseek(File, 0, SEEK_END);
	Size = ftell(File);

	Lump = (ILubyte*)malloc(Size);
	fseek(File, 0, SEEK_SET);
	fread(Lump, 1, Size, File);

	LOG("SIZE: %i", Size);

	if (ilLoadImage(path.c_str()) == true) {

		LOG("FOUND");
		GLuint texID;
		GLuint texture;


		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		texID = ilutGLBindTexImage();

		glBindTexture(GL_TEXTURE_2D, texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		ilDeleteImages(1, &texID);

		glBindTexture(GL_TEXTURE_2D, 0);
		fclose(File);
		free(Lump);

		return texID;
	}

	fclose(File);


	LOG("NOT FOUND");
}