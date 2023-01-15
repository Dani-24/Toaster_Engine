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

#include "ModuleAnimation.h"

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
	uint fileSize = 0;
	char* buffer = nullptr;
	fileSize = app->importer->Load(file_path, &buffer);

	const aiScene* scene = aiImportFileFromMemory(buffer, fileSize, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	if (scene == NULL) {
		LOG("MESH ERROR: Error loading %s file", file_path.c_str());
		return nullptr;
	}

	std::vector<Mesh*> meshes;

	if (scene != nullptr && scene->HasMeshes())
	{
		for (int i = 0; i < scene->mNumMeshes; i++) {
			aiMesh* sceneMesh = new aiMesh();
			sceneMesh = scene->mMeshes[i];

			Mesh* meshData = new Mesh();
			meshData->path = file_path;
			meshData->name = sceneMesh->mName.C_Str();

			Import(sceneMesh, meshData);
			meshes.push_back(meshData);
		}
	}
	else {
		LOG("MESH : This has no meshes or it's cursed -> %s", file_path.c_str());
	}
	
	// Loading Animations

	std::vector<Animation*> animations;

	if (scene != nullptr && scene->HasAnimations()) {
		for (uint i = 0; i < scene->mNumAnimations; i++) {
			animations.push_back(app->anim3d->LoadAnimation(scene->mAnimations[i]));
		}
	}

	// Bones
	if (scene != nullptr) {
		if (scene->mMeshes[0]->HasBones()) {
			ReadNodeHierarchy(scene->mRootNode, meshes[0]);
			NodeToHierarchy();
		}
	}

	if (meshes.size() < 2) {
		if (!animations.empty()) {
			go->AddAnimation(animations);
		}

		// ADD BONES AS CHILD GO
		if (rootBoneNode != nullptr) {
			NodeToGO(rootBoneNode, go, meshes[0]);
		}

		aiReleaseImport(scene);

		return meshes[0];
	}
	else {
		if (go == nullptr) {
			go = app->editor->root;
		}
		for (int i = 0; i < meshes.size(); i++) {
			GameObject* meshChild = new GameObject(meshes[i]->name.c_str(), go);
			meshChild->AddTexture(go->GetTexture());
			meshChild->AddMesh(meshes[i]);
		}
		
		go->DeleteTextures();
		app->editor->SetSelectedGameObject(go);

		aiReleaseImport(scene);
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

	glGenBuffers(1, (GLuint*)&(mesh->id_bonesIDs));
	glGenBuffers(1, (GLuint*)&(mesh->id_bonesWeights));

	//Bind and fill buffers
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertices * VERTEX_ARG, mesh->vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->num_indices, mesh->indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_normals * 3, mesh->normals, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_textureCoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_textureCoords * 2, mesh->textureCoords, GL_STATIC_DRAW);


	/*glBindBuffer(GL_ARRAY_BUFFER, mesh->id_bonesIDs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(int) * mesh->num_bonesIDs * 4, mesh->bonesIDs, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_bonesWeights);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_bonesWeights * 5, mesh->bonesWeights, GL_STATIC_DRAW);*/

	//Unbind buffers
	glDisableClientState(GL_VERTEX_ARRAY);

}

void ModuleMesh3D::Import(const aiMesh* sceneMesh, Mesh* meshData) {
	//copy vertices
	meshData->num_vertices = sceneMesh->mNumVertices;
	meshData->vertices = new float[meshData->num_vertices * VERTEX_ARG];

	memcpy(meshData->vertices, sceneMesh->mVertices, sizeof(float) * meshData->num_vertices * VERTEX_ARG);

	LOG("MESH : New mesh with %d vertices", meshData->num_vertices);

	//copy faces
	if (sceneMesh->HasFaces())
	{
		meshData->num_indices = sceneMesh->mNumFaces * 3;
		meshData->indices = new uint[meshData->num_indices]; // assume each face is a triangle
		for (uint i = 0; i < sceneMesh->mNumFaces; ++i)
		{
			if (sceneMesh->mFaces[i].mNumIndices != 3) {
				LOG("WARNING: geometry face with != 3 indices!");
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

	if (sceneMesh->HasBones())
	{
		LOG("MESH : Loading Mesh Bones");
		meshData->num_bonesIDs = sceneMesh->mNumVertices;
		meshData->num_bonesWeights = sceneMesh->mNumVertices;

		meshData->bonesIDs = new int[4];
		for (uint i = 0; i < 4; i++) meshData->bonesIDs[i] = -1;

		meshData->bonesWeights = new float[5];
		for (uint i = 0; i < 5; ++i) meshData->bonesWeights[i] = .0f;

		meshData->boneTransforms.resize(sceneMesh->mNumBones);

		//iterate all bones
		for (uint boneId = 0; boneId < sceneMesh->mNumBones; boneId++)
		{
			aiBone* aibone = sceneMesh->mBones[boneId];
			meshData->bonesMap[aibone->mName.C_Str()] = boneId;

			//Load offsets
			float4x4 offset = float4x4(aibone->mOffsetMatrix.a1, aibone->mOffsetMatrix.a2, aibone->mOffsetMatrix.a3, aibone->mOffsetMatrix.a4,
				aibone->mOffsetMatrix.b1, aibone->mOffsetMatrix.b2, aibone->mOffsetMatrix.b3, aibone->mOffsetMatrix.b4,
				aibone->mOffsetMatrix.c1, aibone->mOffsetMatrix.c2, aibone->mOffsetMatrix.c3, aibone->mOffsetMatrix.c4,
				aibone->mOffsetMatrix.d1, aibone->mOffsetMatrix.d2, aibone->mOffsetMatrix.d3, aibone->mOffsetMatrix.d4);

			meshData->bonesOffsets.push_back(offset);

			//iterate all bone weights
			for (uint weights = 0; weights < aibone->mNumWeights; weights++) {

				int vertexId = aibone->mWeights[weights].mVertexId; // *4;

				for (int w = 0; w < 4; w++)
				{
					if (meshData->bonesIDs[vertexId * 4 + w] == -1)
					{
						//bone ids
						meshData->bonesIDs[vertexId * 4 + w] = boneId;
						//weights
						meshData->bonesWeights[vertexId * 4 + w] = aibone->mWeights[weights].mWeight;
						break;
					}
				}
			}
		}
	}

	LoadMesh(meshData);
}

void ModuleMesh3D::ReadNodeHierarchy(aiNode* pnode, Mesh* meshData)
{
	string NodeName(pnode->mName.data);

	if (meshData->bonesMap.find(NodeName) != meshData->bonesMap.end()) {
		allBonesNodes.push_back(pnode);

		LOG("MESH : Loading Bone Node %s", NodeName.c_str());
	}

	for (uint i = 0; i < pnode->mNumChildren; i++) {
		ReadNodeHierarchy(pnode->mChildren[i], meshData);
	}
}

void ModuleMesh3D::NodeToHierarchy() {
	rootBoneNode = nullptr;

	int cont = 0;

	// Search root bone
	for (int i = 0; i < allBonesNodes.size(); i++) 
	{
		// See if this node parent is not another node from the list
		for (int j = 0; j < allBonesNodes.size(); j++) {
			if (allBonesNodes[i]->mParent != allBonesNodes[j]) {
				cont++;
			}
		}

		if (cont == allBonesNodes.size()) {
			rootBoneNode = allBonesNodes[i];
			break;
		}

		cont = 0;
	}

	LOG("MESH : Root Bone is %s", rootBoneNode->mName.data);
}

void ModuleMesh3D::NodeToGO(aiNode* root, GameObject* parent, Mesh* parentMesh) 
{
	GameObject* boneGO = new GameObject(root->mName.data, parent);

	if (parentMesh != nullptr) {
		parentMesh->SetRootBone(boneGO);
	}

	vec3 position = vec3(root->mTransformation[0][3], root->mTransformation[1][3], root->mTransformation[2][3]);

	vec3 scale = vec3(	sqrt(root->mTransformation[0][0] * root->mTransformation[0][0] + root->mTransformation[1][0] * root->mTransformation[1][0] + root->mTransformation[2][0] * root->mTransformation[2][0]),
						sqrt(root->mTransformation[0][1] * root->mTransformation[0][1] + root->mTransformation[1][1] * root->mTransformation[1][1] + root->mTransformation[2][1] * root->mTransformation[2][1]),
						sqrt(root->mTransformation[0][2] * root->mTransformation[0][2] + root->mTransformation[1][2] * root->mTransformation[1][2] + root->mTransformation[2][2] * root->mTransformation[2][2]));

	mat4x4 rotation;

	rotation[0] = root->mTransformation[0][0] / scale.x;
	rotation[1] = root->mTransformation[0][1] / scale.y;
	rotation[2] = root->mTransformation[0][2] / scale.z;
	rotation[4] = root->mTransformation[1][0] / scale.x;
	rotation[5] = root->mTransformation[1][1] / scale.y;
	rotation[6] = root->mTransformation[1][2] / scale.z;
	rotation[8] = root->mTransformation[2][0] / scale.x;
	rotation[9] = root->mTransformation[2][1] / scale.y;
	rotation[10] = root->mTransformation[2][2] / scale.z;

	vec3 rotationEuler;

	float sy = sqrt(rotation[0] * rotation[0] + rotation[4] * rotation[4]);

	bool singular = sy < 1e-6; // check for gimbal lock
	float x, y, z;
	if (!singular)
	{
		x = atan2(rotation[9], rotation[10]);
		y = atan2(-rotation[8], sy);
		z = atan2(rotation[4], rotation[0]);
	}
	else
	{
		x = atan2(rotation[6], rotation[5]);
		y = atan2(-rotation[8], sy);
		z = 0;
	}

	rotationEuler = vec3(x, y, z);

	boneGO->SetTransform(position,rotationEuler, scale);
	
	for (int i = 0; i < root->mNumChildren; i++) {
		NodeToGO(root->mChildren[i], boneGO);
	}
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
	TryCalculateBones();

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


	if (calculatedBonesThisFrame && !boneTransforms.empty()) {

		glUniformMatrix4fv(GL_FLOAT, boneTransforms.size(), GL_FALSE, (GLfloat*)&boneTransforms[0]);
		calculatedBonesThisFrame = false;

		//LOG("MESH : Huesos Render?");
	}

	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	boneTransforms.clear();
}

void Mesh::SetRootBone(GameObject* go) {
	if (go == nullptr) {
		LOG("MESH : Trying to assign null root bone");
		return;
	}

	rootBone = go;

	GetBoneMapping();

	boneTransforms.resize(bonesOffsets.size());

}

void Mesh::GetBoneMapping()
{
	/*bonesMap = rootBone->childs;*/
}

void Mesh::TryCalculateBones() {
	if (rootBone == nullptr) {
		return;
	}

	//Mesh array with transform matrix of each bone
	if (calculatedBonesThisFrame == false)
	{
		//float4x4 invertedMatrix = dynamic_cast<C_Transform*>(gameObject->GetComponent(Component::TYPE::TRANSFORM))->globalTransform.Inverted();
		mat4x4 invertedMatrix = inverse(asignedGo->GO_matrix);

		boneTransforms.reserve(bonesMap.size());

		//Get each bone
		for (int i = 0; i < bonesMap.size(); ++i)
		{
			//GameObject* bone = bonesMap[i];

			//if (bone != nullptr)
			//{
			//	//Calcule of Delta Matrix
			//	
			//	mat4x4 Delta = bone->GO_matrix * invertedMatrix;
			//	Delta = Delta * bonesOffsets[i];

			//	//Storage of Delta Matrix (Transformation applied to each bone)
			//	//_mesh->boneTransforms[i] = Delta.Transposed();
			//	boneTransforms.push_back(transpose(Delta));
			//}
		}
		calculatedBonesThisFrame = true;
	}
}