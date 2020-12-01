#include "HeightMapNode.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/TextureManager.h"
#include "../nclgl/MeshManager.h"
#include "../nclgl/ShaderManager.h"

HeightMapNode::HeightMapNode(HeightMap* mesh,	std::string assignMap,
											std::string diffuse1,
											std::string diffuse2,
											std::string normal1,
											std::string normal2) : SceneNode(mesh) {
	this->assignMap = TextureManager::LoadTexture(assignMap, SOIL_FLAG_MIPMAPS);
	this->diffuse[0]  = TextureManager::LoadTexture(diffuse1, SOIL_FLAG_MIPMAPS);
	this->diffuse[1] = TextureManager::LoadTexture(diffuse2, SOIL_FLAG_MIPMAPS);
	this->normal[0] = TextureManager::LoadTexture(normal1, SOIL_FLAG_MIPMAPS);
	this->normal[1] = TextureManager::LoadTexture(normal2, SOIL_FLAG_MIPMAPS);

	TextureManager::SetTextureRepeating(diffuse[0], true);
	TextureManager::SetTextureRepeating(diffuse[1], true);
	TextureManager::SetTextureRepeating(normal[0], true);
	TextureManager::SetTextureRepeating(normal[1], true);

	heightMapSize = mesh->GetHeightMapSize();

	shader = ShaderManager::LoadShader("HeightMapVertex.glsl", "HeightMapFragment.glsl");
}

void HeightMapNode::Draw(const Shader* activeShader) {

	if (mesh) {

		if (activeShader == shader) {
			glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "diffuseTex1"), 0);
			glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "normalTex1"), 1);
			glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "diffuseTex2"), 2);
			glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "normalTex2"), 3);
			glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "assignMap"), 4);
			glUniform3fv(glGetUniformLocation(activeShader->GetProgram(), "dimensions"),1,(float*)&(heightMapSize * worldScale));
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normal[0]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, diffuse[1]);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normal[1]);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, assignMap);

		mesh->Draw();
	}
}