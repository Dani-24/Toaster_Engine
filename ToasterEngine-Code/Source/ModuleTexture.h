#pragma once

#include "Globals.h"
#include "Module.h"

class Texture
{
public:
	Texture();
	~Texture();

	std::string name = "";
	uint OpenGLID = 0;
	int width = 0;
	int height = 0;

	bool bind = true;
};

class ModuleTexture : public Module
{
public:

	ModuleTexture(Application* app, bool start_enabled = true);
	~ModuleTexture();

	// Image file to DDS
	static void ImportImage(const std::string& filename, char* buffer, uint size);

	// Texture from DDS to OpenGL
	static uint Load(char* buffer, int size, int* w, int* h, std::string&& filename);

	static uint CheckImage();

	static uint ImportTexture(std::string path);

	// Managing
	struct BindedTextureInfo
	{
		BindedTextureInfo(uint tID, uint oglID) : TextureID(tID), OpenGLSlotID(oglID) {}
		uint TextureID;
		uint OpenGLSlotID;
	};

	static float BindTexture(uint texture);
	static void UnBindTextures();

	static uint bindedTextures;
	static std::map<uint, Texture> loadedTextures;
	static std::map<std::string, uint> usedPaths;
	static std::vector<BindedTextureInfo> bindedTexturesInfo;
};