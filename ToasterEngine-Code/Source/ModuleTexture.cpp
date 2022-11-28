#include "Application.h"

#include "OpenGL.h"

#include "../External/DevIL/include/il.h"
#include "../External/DevIL/include/ilut.h"

#pragma comment (lib, "External/DevIL/libx86/DevIL.lib")
#pragma comment (lib, "External/DevIL/libx86/ILU.lib")
#pragma comment (lib, "External/DevIL/libx86/ILUT.lib")

#include "ModuleTexture.h"

#include "../External/stb_image/stb_image.cpp"

Texture::Texture()
{
}
Texture::~Texture()
{
}

ModuleTexture::ModuleTexture(Application* app, bool start_enabled) : Module(app, start_enabled){}
ModuleTexture::~ModuleTexture(){}

std::map<uint, Texture> ModuleTexture::loadedTextures;
std::map<std::string, uint> ModuleTexture::usedPaths;
std::vector<ModuleTexture::BindedTextureInfo> ModuleTexture::bindedTexturesInfo;
uint ModuleTexture::bindedTextures = 0;

void ModuleTexture::ImportImage(const std::string& fileName, char* buffer, uint size)
{
	ILuint ImgId = 0;
	ilGenImages(1, &ImgId);
	ilBindImage(ImgId);
	if (!ilLoadL(IL_TYPE_UNKNOWN, buffer, size))
	{
		LOG("Error loading image: %s", ilutGetString(ilGetError()));
	}

	ILuint imgSize;
	ILubyte* data;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5); // To pick a specific DXT compression use
	imgSize = ilSaveL(IL_DDS, nullptr, 0); // Get the size of the data buffer

	if (imgSize > 0)
	{
		data = new ILubyte[imgSize]; // allocate data buffer
		ilSaveL(IL_DDS, data, imgSize); // Save to buffer with the ilSaveIL function

		ModuleImporter::Save(fileName + ".dds", (char*)data, imgSize, false);

		RELEASE_ARRAY(data);
	}

	ilDeleteImages(1, &ImgId);
}

uint ModuleTexture::Load(char* buffer, int size, int* width, int* heigth, std::string&& filename)
{
	//Check if the given texture has been already loaded
	if (ModuleTexture::usedPaths.find(filename) != ModuleTexture::usedPaths.end())
	{
		return ModuleTexture::usedPaths[filename]; // If this texture path was already loaded, return the loaded texture.
	}

	ILuint ImgId = 0;
	ilGenImages(1, &ImgId);
	ilBindImage(ImgId);
	if (!ilLoadL(IL_TYPE_UNKNOWN, buffer, size))
	{
		LOG("Error loading image: %s", ilutGetString(ilGetError()));
	}

	uint error = ilGetError();

	Texture engineTexture;

	engineTexture.width = ilGetInteger(IL_IMAGE_WIDTH);
	engineTexture.height = ilGetInteger(IL_IMAGE_HEIGHT);
	engineTexture.name = filename;

	ILubyte* bytes = ilGetData();

	glGenTextures(1, &engineTexture.OpenGLID);

	//TODO: Generate mipmaps and use best settings
	glBindTexture(GL_TEXTURE_2D, engineTexture.OpenGLID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, engineTexture.width, engineTexture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glBindTexture(GL_TEXTURE_2D, 0);

	ilDeleteImages(1, &ImgId);
	glBindTexture(GL_TEXTURE_2D, 0);

	ModuleTexture::loadedTextures[engineTexture.OpenGLID] = engineTexture; // Add loaded texture inside TextureManager.
	ModuleTexture::usedPaths[filename] = engineTexture.OpenGLID;

	return engineTexture.OpenGLID;
}

uint ModuleTexture::CheckImage()
{
	//Check if the given texture has been already loaded
	if (ModuleTexture::usedPaths.find("Checkers") != ModuleTexture::usedPaths.end())
	{
		return ModuleTexture::usedPaths["Checkers"]; // If this texture path was already loaded, return the loaded texture.
	}

	GLubyte checkerImage[240][240][4];
	for (int i = 0; i < 240; i++) {
		for (int j = 0; j < 240; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	uint textureID = 0;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 240, 240,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	Texture engineTexture;
	engineTexture.OpenGLID = textureID;
	engineTexture.name = "Checkers";
	engineTexture.width = engineTexture.height = 240;

	ModuleTexture::loadedTextures[textureID] = engineTexture; // Add loaded texture inside TextureManager.
	ModuleTexture::usedPaths["Checkers"] = textureID;

	return textureID;
}

uint ModuleTexture::ImportTexture(std::string path)
{
	LOG("Trying to import %s texture", path.c_str());

	//Check if the given texture has been already loaded
	if (ModuleTexture::usedPaths.find(ModuleImporter::GetFileName(path, false)) != ModuleTexture::usedPaths.end())
	{
		return ModuleTexture::usedPaths[ModuleImporter::GetFileName(path, false)]; // If this texture path was already loaded, return the loaded texture.
	}

	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;	// BPP = Bits per pixel

	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (m_LocalBuffer)
	{
		stbi_image_free(m_LocalBuffer);
	}

	Texture engineTexture;
	engineTexture.OpenGLID = m_RendererID;
	engineTexture.name = path;

	ModuleTexture::loadedTextures[m_RendererID] = engineTexture; // Add loaded texture inside TextureManager.
	ModuleTexture::usedPaths[ModuleImporter::GetFileName(path, false)] = m_RendererID;

	return m_RendererID;
}

float ModuleTexture::BindTexture(uint texture)
{
	for (int i = 0; i < bindedTexturesInfo.size(); i++)
	{
		if (bindedTexturesInfo[i].TextureID == texture)
		{
			return bindedTexturesInfo[i].OpenGLSlotID; // If binding texture has already been binded, return the OpenGl slot
		}
	}

	if (GL_TEXTURE0 + bindedTextures == GL_TEXTURE31) // If we loaded 32 textures already
		return -1;

	glActiveTexture(GL_TEXTURE0 + bindedTextures);
	glBindTexture(GL_TEXTURE_2D, texture);

	bindedTexturesInfo.push_back({ texture, bindedTextures });

	return bindedTextures++;
}

void ModuleTexture::UnBindTextures()
{
	bindedTextures = 0;
	bindedTexturesInfo.clear();
	for (int i = 0; i < 32; i++)
	{
		glBindTexture(GL_TEXTURE0 + i, 0); // Unbind every texture
	}
}