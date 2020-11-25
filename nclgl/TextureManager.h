#pragma once

#include <map>
#include <string>
#include <glad/glad.h>

static class TextureManager {

	static std::map<std::string, GLuint> loadedTextures;

public:
	static GLuint	LoadTexture(std::string textureName, unsigned int flags = 0);
	static void		SetTextureRepeating(GLuint texture, bool repeat);
	static void		Cleanup();

};