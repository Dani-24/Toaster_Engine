#pragma once

#include "../External/MathGeoLib/include/Math/float2.h"

#include "OpenGL.h"
#include "Globals.h"

class Buffer
{
public:
	Buffer();
	~Buffer() {};

	void BindBuffer();

	void SetBufferDimensions(int width, int height);
	void SetBufferInfo();
	void RechargeBuffer();

	uint GetTexture() { return textureBuff; }

private:
	uint frameBuff = 0;
	uint textureBuff = 0;
	uint renderBuff = 0;

	int width, height;

	friend class Camera;
};