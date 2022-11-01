#pragma once

#include "Globals.h"
#include "Module.h"

#include "FileTree.cpp"

enum class ResourceType
{
	SUSSYFILE,
	MESH,
	TEXTURE
};

class ModuleImporter : public Module
{
public:
	ModuleImporter(Application* app, bool start_enabled = true);
	~ModuleImporter();

	static bool DoTheFileExists(const std::string file);

	static bool CreateFileDirectory(const std::string dir);

	static std::string NormalizePath(const std::string path);

	static std::string UnNormalizePath(const std::string path);

	// can be path or zip
	static bool AddPathToFileSystem(const std::string path);

	static uint Load(const std::string filePath, char** buffer);

	static uint Save(const std::string filePath, char* buffer, uint size, bool append);

	static bool Copy(const std::string src, const std::string des, bool replace = true);

	static FileTree* GetFileTree(std::string path, FileTree* parent = nullptr);

	static std::string GetFileName(const std::string file, bool getExtension = true);

	static ResourceType GetResourceType(const std::string& filename);

	static std::vector<std::string> GetAllFiles(std::string path);
};