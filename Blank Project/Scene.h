#pragma once

class SceneNode;
class Camera;

class Scene {

public:
	
	friend class Renderer;

	Scene();
	~Scene();

//	virtual void Update(float dt);

protected:
	SceneNode* root;
	Camera* camera;
};