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

	Shader* defaultShader;
	Shader* bumpMapShader;
	Shader* animatedShader;
	Shader* bumpAnimatedShader;


	Mesh* mesh_roleT;
	MeshAnimation* anim_roleT;
	MeshMaterial* mat_roleT;

	Mesh* mesh_cyberSoldier;
	MeshAnimation* anim_cyberSoldier;
	MeshMaterial* mat_cyberSoldier;

	HeightMap* mesh_heightMap;
	GLuint diffuse_heightMap;
	GLuint normal_heightMap;

	Mesh* mesh_Barrel;
	MeshMaterial* mat_Barrel;	
};