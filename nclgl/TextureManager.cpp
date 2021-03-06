#include "TextureManager.h"
#include "SOIL/SOIL.h"

std::map<std::string, GLuint> TextureManager::loadedTextures;

GLuint TextureManager::LoadTexture(std::string textureName, unsigned int flags,bool linearFilter, bool aniso) {
	
	//If we have loaded this texture in the past, just return it again.
	if (loadedTextures.find(textureName) != loadedTextures.end())
		return loadedTextures[textureName];

	GLuint newTexture = SOIL_load_OGL_texture(textureName.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, flags);

	if (!newTexture)
		return 0;

	glBindTexture(GL_TEXTURE_2D, newTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linearFilter ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linearFilter ? GL_LINEAR :GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, aniso ? 16.0f : 1.0f);
	glBindTexture(GL_TEXTURE_2D, 0);

	loadedTextures.emplace(textureName, newTexture);

	return newTexture;
}

GLuint TextureManager::LoadCubemap(std::string xPos, std::string xNeg, std::string yPos, std::string yNeg, std::string zPos, std::string zNeg, unsigned int flags) {

	std::string combined = xPos + xNeg + yPos + yNeg + zPos + zNeg;

	//If we have loaded this cubemap in the past, just return it again.
	if (loadedTextures.find(combined) != loadedTextures.end())
		return loadedTextures[combined];

	GLuint newCubemap = SOIL_load_OGL_cubemap(
		xPos.c_str(), xNeg.c_str(),
		yPos.c_str(), yNeg.c_str(),
		zPos.c_str(), zNeg.c_str(),
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, flags);

	if (!newCubemap)
		return 0;

	loadedTextures.emplace(combined, newCubemap);

	return newCubemap;

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