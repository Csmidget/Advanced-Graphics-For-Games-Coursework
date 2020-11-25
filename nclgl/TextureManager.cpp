#include "TextureManager.h"
#include "SOIL/SOIL.h"

std::map<std::string, GLuint> TextureManager::loadedTextures;

GLuint TextureManager::LoadTexture(std::string textureName, unsigned int flags) {
	
	//If we have loaded this texture in the past, just return it again.
	if (loadedTextures.find(textureName) != loadedTextures.end())
		return loadedTextures[textureName];

	GLuint newTexture = SOIL_load_OGL_texture(textureName.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, flags);

	loadedTextures.emplace(textureName, newTexture);

	return newTexture;
}

void TextureManager::SetTextureRepeating(GLuint texture, bool repeat)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureManager::Cleanup() {
	for (auto const t : loadedTextures)	{
		glDeleteTextures(1, &t.second);
	}
	loadedTextures.clear();
}