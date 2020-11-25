#include "Scene.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Light.h"

Scene::Scene() {
	initialized = false;
	camera = new Camera(0.0f, 0.0f, 0.0f, Vector3(0, 500.0f, 750.0f));
	root = new SceneNode();
}

Scene::~Scene() {
	delete camera;
	delete root;
}