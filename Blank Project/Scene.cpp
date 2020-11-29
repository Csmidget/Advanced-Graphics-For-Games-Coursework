#include "Scene.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Light.h"

Scene::Scene() {
	initialized = false;
	camera = new Camera(0.0f, 0.0f, 0.0f, Vector3(0, 500.0f, 750.0f),1.0f, 10000.0f,45.0f);
	root = new SceneNode();
	skybox = 0;
}

Scene::~Scene() {
	delete camera;
	delete root;
}

void Scene::Update(float dt) {
	root->Update(dt);
}

Matrix4 Scene::GetCameraPerspective(int width, int height) {
	return Matrix4::Perspective(camera->GetNearPlane(), camera->GetFarPlane(), (float)width / (float)height, camera->GetFOV());
}