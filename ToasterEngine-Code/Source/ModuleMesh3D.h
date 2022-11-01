#pragma once
#include "Module.h"
#include "Globals.h"

#include "../External/Assimp/include/cimport.h"
#include "../External/Assimp/include/scene.h"
#include "../External/Assimp/include/postprocess.h"

#include <string>

using namespace std;

#define VERTEX_ARG 5

struct Mesh {

	Mesh() {}
	~Mesh();

	uint id_indices = 0;
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertices = 0;
	uint num_vertices = 0;
	float* vertices = nullptr;

	void Render();
};

class ModuleMesh3D : public Module
{
public:
	ModuleMesh3D(Application* app, bool start_enabled = true);
	~ModuleMesh3D();

	bool Start();
	update_status PostUpdate(float dt);
	bool CleanUp();

	Mesh* LoadFile(string file_path);
	void LoadMesh(Mesh* mesh);

	uint LoadTexture(string file_path);

private:

	vector<Mesh*> meshes;
};