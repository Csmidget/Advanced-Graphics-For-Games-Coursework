#pragma once

#include "../nclgl/Matrix4.h"
#include <glad/glad.h>
#include <vector>

class SceneNode;
class Camera;
class PointLight;
class SpotLight;

class Scene {

public:
	
	friend class Renderer;

	Scene();
	~Scene();

	virtual void Update(float dt);

	Matrix4 GetCameraPerspective(int width, int height);
	void GenerateRandomLights(int pointLightCount, int spotLightCount, Vector3 minPos, Vector3 maxPos);

protected:
	bool initialized;
	SceneNode* root;
	Camera* camera;
	GLuint skybox;

	std::vector<PointLight*> pointLights;
	std::vector<SpotLight*> spotLights;
};