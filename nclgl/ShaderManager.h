#pragma once

#include <map>
#include <string>
#include "Shader.h"

class ShaderManager {

	friend class OGLRenderer;

	static std::map<std::string, Shader*> loadedShaders;
	static void		Cleanup();

public:
	static Shader* LoadShader(std::string shaderVert, std::string shaderFrag, std::string shaderGeom = "");
};