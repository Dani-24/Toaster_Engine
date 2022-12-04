#pragma once
#include "Module.h"
#include "Globals.h"

#include "../External/Assimp/include/cimport.h"
#include "../External/Assimp/include/scene.h"
#include "../External/Assimp/include/postprocess.h"

#include <string>

using namespace std;

#define VERTEX_ARG 3

class GameObject;
struct Mesh {

	Mesh() {}
	~Mesh();

	uint id_indices = 0;
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertices = 0;
	uint num_vertices = 0;
	float* vertices = nullptr;

	uint id_normals = 0;
	uint num_normals = 0;
	float* normals = nullptr;

	uint id_textureCoords = 0;
	uint num_textureCoords = 0;
	float* textureCoords = nullptr;

	string name;
	string path;

	void Render(uint texture, mat4x4 matrix);
};

class ModuleMesh3D : public Module
{
public:
	ModuleMesh3D(Application* app, bool start_enabled = true);
	~ModuleMesh3D();

	bool Start();
	update_status PostUpdate(float dt);
	bool CleanUp();

	Mesh* LoadFile(string file_path, GameObject* go = nullptr);
	void LoadMesh(Mesh* mesh);

	void Import(const aiMesh* sceneMesh, Mesh* meshData);

private:

	//vector<Mesh*> meshes;
};