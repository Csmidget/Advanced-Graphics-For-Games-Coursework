#pragma once

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

	virtual void Update(float dt) = 0;

protected:
	bool initialized;
	SceneNode* root;
	Camera* camera;
	GLuint skybox;

	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;
};