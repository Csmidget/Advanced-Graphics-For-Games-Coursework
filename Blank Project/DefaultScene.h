#pragma once

#include "Scene.h"
#include "CameraTrack.h"
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

	CameraTrack* track;

	Shader* defaultShader;
	Shader* bumpMapShader;
	Shader* animatedShader;
	Shader* reflectShader;

	SceneNode* water;
	Mesh* waterMesh;

	HeightMap* heightMapMesh;

	float waterRotate;
	float waterCycle = 0.0f;
};