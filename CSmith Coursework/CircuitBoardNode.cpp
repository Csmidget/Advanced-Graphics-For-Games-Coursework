#include "CircuitBoardNode.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/TextureManager.h"
#include "../nclgl/MeshManager.h"
#include "../nclgl/ShaderManager.h"

CircuitBoardNode::CircuitBoardNode(Mesh* mesh) : SceneNode(mesh) {

	Shader* defShader = ShaderManager::LoadShader("BufferBumpVertex.glsl", "CircuitFragment.glsl");
	SetShader(defShader);

	this->background = TextureManager::LoadTexture(TEXTUREDIR"metall010.png", SOIL_FLAG_MIPMAPS);
	SetNormal(TextureManager::LoadTexture(TEXTUREDIR"CircuitNormal.png", SOIL_FLAG_MIPMAPS));
	this->circuit = TextureManager::LoadTexture(TEXTUREDIR"circuit.png", SOIL_FLAG_MIPMAPS);
	this->overlay = TextureManager::LoadTexture(TEXTUREDIR"doge.png", SOIL_FLAG_MIPMAPS);

	TextureManager::SetTextureRepeating(background, true);
	TextureManager::SetTextureRepeating(circuit, true);
	TextureManager::SetTextureRepeating(overlay, true);

	//The floor mesh has funky texcoords, this recenters and scales the texture
	this->SetTextureMatrix(Matrix4::Translation({-1, -0.5, 0 }) * Matrix4::Scale({ 2,2,1 }));
	texOffset = 0;
}

void CircuitBoardNode::Update(float dt) {
	texOffset += dt * 0.5f;

	if (texOffset > 1)
		texOffset -= 1;

	SceneNode::Update(dt);
}

void CircuitBoardNode::Draw(const Shader* activeShader) {
	if (mesh) {

		if (activeShader == shader) {
			glUniform1f(glGetUniformLocation(activeShader->GetProgram(), "offset"), texOffset);
			glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "baseTex"), 0);
			glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "circuitTex"), 1);
			glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "overlayTex"), 2);
			glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "normalTex"), 3);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, background);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, circuit);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, overlay);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, normal);
		}

		SceneNode::Draw(activeShader);
	}
}