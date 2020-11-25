#pragma once

#include "Scene.h"
#include <glad/glad.h>

class Mesh;
class MeshMaterial;
class MeshAnimation;
class Shader;
class HeightMap;

class DefaultScene : public Scene {
public:
	DefaultScene();
	~DefaultScene();

	void Update(float dt) override;

	Mesh* mesh_roleT;
	MeshAnimation* anim_roleT;
	MeshMaterial* mat_roleT;
	Shader* shader_roleT;

	HeightMap* mesh_heightMap;
	Shader* heightMapShader;
	GLuint heightMapTexture;
	GLuint heightMapBump;
};