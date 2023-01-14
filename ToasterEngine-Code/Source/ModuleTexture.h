#pragma once

#include "Application.h"

#include <vector>
#include <string>

#include "Module.h"

typedef unsigned int GLuint;
class Resource;
class DE_Cubemap;

class ModuleTexture : public Module
{
public:
	ModuleTexture(Application* app, bool start_enabled = true);
	~ModuleTexture();

	bool Init();

	GLuint LoadToMemory(char* buffer, int size, int* w = nullptr, int* h = nullptr);
	void SaveDDS(char* buffer, int size, const char* fileName);

	void Import(char* buffer, int bSize, Resource* res);
};