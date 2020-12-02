#include "Scene.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Light.h"

Scene::Scene() {
	initialized = false;
	camera = new Camera({0, 500.0f, 750.0f}, { 0,0,0 }, 1.0f, 10000.0f, 45.0f);
	root = new SceneNode();
	skybox = 0;
}

Scene::~Scene() {
	delete camera;
	delete root;

	for (int i = 0; i < spotLights.size(); i++) {
		delete spotLights[i];
	}

	for (int i = 0; i < pointLights.size(); i++) {
		delete pointLights[i];
	}
}

void Scene::Update(float dt) {
	root->Update(dt);
}

Matrix4 Scene::GetCameraPerspective(int width, int height) {
	return Matrix4::Perspective(camera->GetNearPlane(), camera->GetFarPlane(), (float)width / (float)height, camera->GetFOV());
}

float RandZeroToOne() {
	return (float)rand() / (float)RAND_MAX;
}

//This should only be called before static shadowmaps are baked.
void Scene::GenerateRandomLights(int pointLightCount, int spotLightCount, Vector3 minPos, Vector3 maxPos) {

	//Light setup
	pointLights.reserve(pointLights.size() * pointLightCount);
	spotLights.reserve(spotLights.size() * spotLightCount);
	Vector3 range = maxPos - minPos;

	for (int i = 0; i < pointLightCount; i++)
	{
		PointLight* l = new PointLight();
		l->SetPosition(Vector3(	RandZeroToOne() * range.x + (int)minPos.x,
								RandZeroToOne() * range.y + (int)minPos.y,
								RandZeroToOne() * range.z + (int)minPos.z));

		l->SetRadius(80.0);

		l->SetDiffuseColour(Vector4(RandZeroToOne() * 2,
									RandZeroToOne() * 2,
									RandZeroToOne() * 2,
									1));
		l->MakeStatic();

		pointLights.emplace_back(l);
	}

	for (int i = 0; i < spotLightCount; ++i) {
		SpotLight* l = new SpotLight();

		l->SetPosition(Vector3(	RandZeroToOne() * range.x + (int)minPos.x,
								RandZeroToOne() * range.y + (int)minPos.y,
								RandZeroToOne() * range.z + (int)minPos.z));

		l->SetDiffuseColour(Vector4(RandZeroToOne() * 2,
									RandZeroToOne() * 2,
									RandZeroToOne() * 2,
									1));

		l->SetRadius(75);
		l->SetAngle(20 + rand() % 45);
		l->MakeStatic();
		spotLights.emplace_back(l);
	}
	////////////
}