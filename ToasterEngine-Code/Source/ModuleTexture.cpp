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

void ModuleTexture::ImportImageFromExternalFolder(const std::string& fileName, char* buffer, uint size)
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

		ModuleImporter::Save(fileName + ".dds", (char*)data, imgSize);

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

Texture* ModuleTexture::CheckersImage()
{
	checkersTexture = LoadTexture("Assets/default_Meshes/Checkers.png");
	return checkersTexture;
}

Texture* ModuleTexture::LoadTexture(std::string path) {
	
	Texture* houseText = new Texture();
	houseText->OpenGLID = ImportTexture(path);
	houseText->name = path;

	return houseText;
}

uint ModuleTexture::ImportTexture(std::string path)
{
	//Check if the given texture has been already loaded
	if (ModuleTexture::usedPaths.find(ModuleImporter::GetFileName(path, false)) != ModuleTexture::usedPaths.end())
	{
		return ModuleTexture::usedPaths[ModuleImporter::GetFileName(path, false)]; // If this texture path was already loaded, return the loaded texture.
	}

	unsigned int desired_texture;
	unsigned char* localBuffer;
	int w, h, bitsXPixel;

	stbi_set_flip_vertically_on_load(1);
	localBuffer = stbi_load(path.c_str(), &w, &h, &bitsXPixel, 4);

	glGenTextures(1, &desired_texture);
	glBindTexture(GL_TEXTURE_2D, desired_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (localBuffer)
	{
		stbi_image_free(localBuffer);
	}

	Texture engineTexture;
	engineTexture.OpenGLID = desired_texture;
	engineTexture.name = path;

	ModuleTexture::loadedTextures[desired_texture] = engineTexture; // Add loaded texture inside TextureManager.
	ModuleTexture::usedPaths[ModuleImporter::GetFileName(path, false)] = desired_texture;

	//int error = glGetError();
	//
	//if (error) {
	//	LOG("Error loading texture %s", path.c_str());
	//	switch (error)
	//	{
	//	case GL_INVALID_ENUM:
	//		LOG("GL ERROR : Invalid enum");
	//		break;
	//	case GL_INVALID_VALUE:
	//		LOG("GL ERROR : Invalid value");
	//		break;
	//	case GL_INVALID_OPERATION:
	//		LOG("GL ERROR : Invalid operation");
	//		break;
	//	case GL_NO_ERROR:
	//		LOG("GL ERROR : No error");
	//		break;
	//	case GL_STACK_OVERFLOW:
	//		LOG("GL ERROR : Stack overflow.com");
	//		break;
	//	case GL_STACK_UNDERFLOW:
	//		LOG("GL ERROR : Stack underflow");
	//		break;
	//	case GL_OUT_OF_MEMORY:
	//		LOG("GL ERROR : Out of memory");
	//		break;
	//	}		
	//}
	//else{
	//	LOG("Loaded %s as %d", engineTexture.name.c_str(), desired_texture);
	//}

	return desired_texture;
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