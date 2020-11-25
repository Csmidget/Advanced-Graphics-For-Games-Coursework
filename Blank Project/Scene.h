#pragma once

#include <vector>

class SceneNode;
class Camera;
class Light;
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

	std::vector<Light> pointLights;
	std::vector<SpotLight> spotLights;
};