#pragma once

#include <map>
#include <string>
#include <glad/glad.h>

static class TextureManager {

	friend class OGLRenderer;

	static std::map<std::string, GLuint> loadedTextures;
	static void		Cleanup();

public:
	static GLuint	LoadTexture(std::string textureName, unsigned int flags = 0);
	static GLuint	LoadCubemap(std::string xPos, std::string xNeg, std::string yPos, std::string yNeg, std::string zPos, std::string zNeg, unsigned int flags = 0);
	static void		SetTextureRepeating(GLuint texture, bool repeat);


};