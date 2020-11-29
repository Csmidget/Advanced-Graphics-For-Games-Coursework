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

	bool rotateLights;

	Shader* defaultShader;
	Shader* bumpMapShader;
	Shader* animatedShader;
	Shader* reflectShader;
	
	Mesh* roleTMesh;
	MeshAnimation* roleTAnim;
	MeshMaterial* roleTMat;

	SceneNode* water;
	Mesh* waterMesh;
	GLuint waterDiffuse;
	GLuint waterNormal;

	HeightMap* heightMapMesh;
	GLuint heighMapDiffuse;
	GLuint heightMapNormal;

	Mesh* barrelMesh;
	MeshMaterial* barrelMat;	

	float waterRotate;
	float waterCycle = 0.0f;
};