#pragma once

#include <map>
#include <string>
#include <glad/glad.h>

static class TextureManager {

	static std::map<std::string, GLuint> loadedTextures;

	static GLuint LoadTexture(std::string textureName);
};