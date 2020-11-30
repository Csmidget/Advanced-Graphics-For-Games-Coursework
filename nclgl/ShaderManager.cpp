#include "ShaderManager.h"

std::map<std::string, Shader*> ShaderManager::loadedShaders;

Shader* ShaderManager::LoadShader(std::string shaderVert, std::string shaderFrag, std::string shaderGeom) {

	std::string combinedName = shaderVert + shaderFrag + shaderGeom;

	//If we have loaded this texture in the past, just return it again.
	if (loadedShaders.find(combinedName) != loadedShaders.end())
		return loadedShaders[combinedName];

	Shader* newShader = new Shader(shaderVert, shaderFrag, shaderGeom); 

	if (!newShader->LoadSuccess()) {
		delete newShader;
		return nullptr;
	}

	loadedShaders.emplace(combinedName, newShader);

	return newShader;
}

void ShaderManager::Cleanup() {
	for (auto  t : loadedShaders) {
		delete t.second;
	}
	loadedShaders.clear();
}