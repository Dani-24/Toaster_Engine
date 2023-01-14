#pragma once

#include "Globals.h"
#include "Module.h"

#include "FileTree.cpp"

enum class ResourceType
{
	SUSSYFILE,
	MESH,
	TEXTURE,
	ANIMATION
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

	static bool AddPathToFileSystem(const std::string path);

	static uint Load(const std::string filePath, char** buffer);
	static void Save(const std::string desiredFilePath, char* buffer, uint size);
	static bool Copy(const std::string src, const std::string des, bool replace = true);

	static FileTree* GetFileTree(std::string path, FileTree* parent = nullptr);
	static std::string GetFileName(const std::string file, bool getExtension = true);

	static ResourceType GetResourceType(const std::string& filename);

	static std::vector<std::string> GetAllFiles(std::string path);

	void DragDropFile(const char* path);

	uint FileToBuffer(const char* filePath, char** fileBuffer)const;
};