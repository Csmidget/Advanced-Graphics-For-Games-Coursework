#pragma once

#include "Scene.h"
#include "Track.h"
#include <glad/glad.h>

#include "../nclgl/Camera.h"

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

	bool rotateLight;

	Track<Camera>* track;
	std::vector<Track<SceneNode>*> patrols;

	SceneNode* water;
	Mesh* waterMesh;

	HeightMap* heightMapMesh;

	SpotLight* spinningLight;
	SpotLight* cameraLight;

	float waterRotate;
	float waterCycle = 0.0f;
};