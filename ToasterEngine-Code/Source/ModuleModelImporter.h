#pragma once
#include <vector>

class GameObject;
class Resource;
class ResourceMesh;
class ResourceAnimation;
class aiScene;
typedef unsigned int uint;

namespace ModelImporter
{
	void Import(char* buffer, int bSize, Resource* res);
}