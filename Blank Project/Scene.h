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

protected:
	bool initialized;
	SceneNode* root;
	Camera* camera;
	GLuint skybox;

	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;
};