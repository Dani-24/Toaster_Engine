#include "Buffer.h"

#include "Application.h"
#include "ModuleWindow.h"

Buffer::Buffer()
{
	width = app->window->width;
	height = app->window->height;
}

void Buffer::BindBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuff);
}

void Buffer::SetBufferDimensions(int width, int height)
{
	this->width = width;
	this->height = height;
	RechargeBuffer();
}

void Buffer::SetBufferInfo()
{
	glGenFramebuffers(1, &frameBuff);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuff);

	glGenTextures(1, &textureBuff);
	glBindTexture(GL_TEXTURE_2D, textureBuff);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuff, 0);

	glGenRenderbuffers(1, &renderBuff);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuff);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuff);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Buffer::RechargeBuffer()
{
	glDeleteFramebuffers(1, &frameBuff);
	glDeleteTextures(1, &textureBuff);
	glDeleteRenderbuffers(1, &renderBuff);

	SetBufferInfo();
}