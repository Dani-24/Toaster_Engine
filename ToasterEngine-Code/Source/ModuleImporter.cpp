#include "ModuleImporter.h"
#include "../External/PhysFS/include/physfs.h"

#include <algorithm>

#pragma comment (lib, "External/PhysFS/libx86/physfs.lib")

ModuleImporter::ModuleImporter(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	PHYSFS_init(0);

	if (PHYSFS_setWriteDir(".") == 0) LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());

	CreateFileDirectory(".");
	CreateFileDirectory("C:/");
	CreateFileDirectory("D:/");

	AddPathToFileSystem(".");
	AddPathToFileSystem("C:/");
	AddPathToFileSystem("D:/");
}

ModuleImporter::~ModuleImporter()
{
	PHYSFS_deinit();
}

bool ModuleImporter::DoTheFileExists(const std::string file)
{
	return PHYSFS_exists(file.c_str()) != 0;
}

bool ModuleImporter::CreateFileDirectory(const std::string dir)
{
	if (PHYSFS_isDirectory(dir.c_str()) != 0)
	{
		PHYSFS_mkdir(dir.c_str());
		return true;
	}

	return false;
}

std::string ModuleImporter::NormalizePath(const std::string path)
{
	std::string ret = path;

	int pos = ret.find("C:\\");

	if (pos != std::string::npos) ret.erase(pos, 3);

	pos = ret.find("D:\\");

	if (pos != std::string::npos) ret.erase(pos, 3);

	for (int i = 0; i < ret.size(); i++)
	{
		if (ret[i] == '\\') ret[i] = '/';
	}

	return ret;
}

std::string ModuleImporter::UnNormalizePath(const std::string path)
{
	std::string ret = path;

	for (int i = 0; i < ret.size(); i++)
	{
		if (ret[i] == '/') ret[i] = '\\';
	}

	return ret;
}

bool ModuleImporter::AddPathToFileSystem(const std::string path)
{
	bool ret = false;

	if (PHYSFS_mount(path.c_str(), nullptr, 1) == 0)
	{
		LOG("File System error while adding a path or zip: %s\n", PHYSFS_getLastError());
	}
	else
		ret = true;

	return ret;
}

uint ModuleImporter::Load(const std::string filePath, char** buffer)
{
	uint byteCount = 0;

	PHYSFS_file* fsFile = PHYSFS_openRead(filePath.c_str());

	do
	{
		if (!fsFile)
		{
			LOG("FILE SYSTEM: error while opening file %s: %s\n", filePath.c_str(), PHYSFS_getLastError());

			break;
		}

		PHYSFS_sint64 size = PHYSFS_fileLength(fsFile);

		if (size <= 0)
		{
			LOG("FILE SYSTEM: error while reading from file %s: %s\n", filePath.c_str(), PHYSFS_getLastError());

			break;
		}

		*buffer = new char[size + 1];

		byteCount = (uint)PHYSFS_readBytes(fsFile, *buffer, size);

		if (byteCount != size)
		{
			LOG("FILE SYSTEM: error while reading from file %s: %s\n", filePath.c_str(), PHYSFS_getLastError());
			RELEASE_ARRAY(*buffer);
			break;
		}

		(*buffer)[size] = '\0';

	} while (false);

	if (PHYSFS_close(fsFile) == 0) LOG("FILE SYSTEM: error while closing file %s: %s\n", filePath.c_str(), PHYSFS_getLastError());

	return byteCount;
}

void ModuleImporter::Save(const std::string desiredFilePath, char* buffer, uint size)
{
	PHYSFS_file* physfs_lib_file = nullptr;
	physfs_lib_file = PHYSFS_openWrite(desiredFilePath.c_str());

	if (physfs_lib_file != nullptr) {
		PHYSFS_writeBytes(physfs_lib_file, (const void*)buffer, size);
		PHYSFS_close(physfs_lib_file);
	}
	else {
		LOG("FILE SYSTEM: ERROR saving an imported file to %s", desiredFilePath.c_str());
	}
}

bool ModuleImporter::Copy(const std::string src, std::string des, bool replace)
{
	bool successful = true;

	bool isDir = PHYSFS_isDirectory(src.c_str());

	if (!isDir)
	{
		std::string fileName = GetFileName(src, true);

		des += fileName;

		char* buffer = nullptr;

		do
		{
			if (DoTheFileExists(des) && !replace)
			{
				LOG("FILE SYSTEM: the file you want to copy is already exist and you don't want to replace this: '%s'", src.c_str());
				successful = false;
				break;
			}

			uint srcSize = Load(src, &buffer);

			if (srcSize <= 0)
			{
				if (DoTheFileExists(src))
				{
					Save(des, buffer, srcSize);
					break;
				}

				LOG("FILE SYSTEM: Could not read from file '%s'", src.c_str());
				successful = false;
				break;
			}

			Save(des, buffer, srcSize);

			LOG("FILE SYSTEM: Successfully copied source file: '%s' to the destination file: '%s'", src.c_str(), des.c_str());

		} while (false);

		RELEASE(buffer);
	}

	return successful;
}

std::string ModuleImporter::GetFileName(const std::string file, bool getExtension)
{
	uint pos = file.find_last_of("/");

	std::string name = file;

	if (pos != std::string::npos) name = file.substr(pos + 1, file.size() - 1);
	else name = file;

	if (!getExtension)
	{
		uint ePos = name.find(".");
		if (ePos != std::string::npos) name = name.substr(0, ePos);
	}

	return name;
}

ResourceType ModuleImporter::GetResourceType(const std::string& filename)
{
	std::string fileExtension = filename;
	fileExtension = fileExtension.substr(fileExtension.find_last_of('.') + 1);

	std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);

	if (fileExtension == "fbx" || fileExtension == "dae" || fileExtension == "FBX") return ResourceType::MESH;
	if (fileExtension == "tga" || fileExtension == "png" || fileExtension == "jpg" || fileExtension == "dds") return ResourceType::TEXTURE;

	return ResourceType::SUSSYFILE;
}

FileTree* ModuleImporter::GetFileTree(std::string path, FileTree* parent)
{
	FileTree* ret = new FileTree(path + "/", GetFileName(path), parent);

	char** list = PHYSFS_enumerateFiles(path.c_str());

	for (int i = 0; list[i] != nullptr; i++)
	{
		std::string dirCheck = path + "/" + list[i];

		if (PHYSFS_isDirectory(dirCheck.c_str()) != 0)
		{
			ret->directories.emplace_back(GetFileTree(dirCheck, ret));
		}
		else
		{
			ret->files.emplace_back(list[i]);
		}
	}

	return ret;
}

std::vector<std::string> ModuleImporter::GetAllFiles(std::string path)
{
	std::vector<std::string> ret;

	char** list = PHYSFS_enumerateFiles(path.c_str());

	for (int i = 0; list[i] != nullptr; i++)
	{
		std::string dirCheck = path + "/" + list[i];

		if (PHYSFS_isDirectory(dirCheck.c_str()) != 0)
		{
			std::vector<std::string> temp = GetAllFiles(dirCheck);
			ret.insert(ret.end(), temp.begin(), temp.end());
		}
		else
		{
			ret.push_back(list[i]);
		}
	}

	return ret;
}

void ModuleImporter::DragDropFile(const char* path) {
	
	std::string importedPath = path;
	std::string fileName = importedPath.substr(importedPath.find_last_of('\\') + 1);
	std::string fileExtension = fileName.substr(fileName.find_last_of('.') + 1);
	std::string destinationPath = "Assets/";

	const char* fileName = fileName.c_str();

	std::string finalAssetPath = destinationPath + fileName;

	LOG("FILE SYSTEM: Adding file: %s", finalAssetPath.c_str());

	//AddPathToFileSystem(dropped_filedir_s.c_str());

	std::FILE* file;
	fopen_s(&file, path, "rb");

	fseek(file, 0, SEEK_END);
	uint size = ftell(file);
	rewind(file);

	char* buffer = new char[size];

	size = fread(buffer, 1, size, file);

	uint bufferSize = FileToBuffer(fileName.c_str(), &buffer);

	if (size > 0)
	{
		Save(finalAssetPath, buffer, size);

		if (buffer != nullptr) {
			delete[] buffer;
			buffer = nullptr;
		}
	}

	fclose(file);
}

uint ModuleImporter::FileToBuffer(const char* filePath, char** fileBuffer) const {

	uint ret = 0;

	PHYSFS_file* pysfsFile = PHYSFS_openRead(filePath);

	if (pysfsFile != nullptr)
	{
		PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(pysfsFile);

		if (size > 0)
		{
			*fileBuffer = new char[size + 1];
			uint readData = (uint)PHYSFS_read(pysfsFile, *fileBuffer, 1, size);

			if (readData == size)
			{
				ret = readData;
				(*fileBuffer)[size] = '\0';

			}
		}

		PHYSFS_close(pysfsFile);
	}

	return ret;
}