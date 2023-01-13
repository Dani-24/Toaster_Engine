#pragma once

#include "Resource.h"
#include <string>
#include "Color.h"

struct SimpleRect
{
	int x, y, w, h;
	SimpleRect(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}
	SimpleRect() : x(0), y(0), w(0), h(0) {}
	bool SimpleRect::Contains(SimpleRect b)
	{
		if ((b.x + b.w) <= (x + w)
			&& (b.x) >= (x)
			&& (b.y) >= (y)
			&& (b.y + b.h) <= (y + h)
			)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

typedef unsigned int GLuint;


class ResourceTexture : public Resource
{
public:
	ResourceTexture(unsigned int _uid);
	~ResourceTexture();

	//bool LoadToMemory() override;
	bool UnloadFromMemory() override;
	SimpleRect GetTextureChunk(SimpleRect area);

public:
	int tWidth;
	int tHeight;

	GLuint textureID;
	Color color;
};
