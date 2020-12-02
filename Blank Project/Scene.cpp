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

float RandZeroToOne() {
	return (float)rand() / (float)RAND_MAX;
}

Matrix4 Scene::GetCameraPerspective(int width, int height) {
	return Matrix4::Perspective(camera->GetNearPlane(), camera->GetFarPlane(), (float)width / (float)height, camera->GetFOV());
}
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

		l->SetRadius(75.0);

		l->SetDiffuseColour(Vector4(0.5f + RandZeroToOne(),
							0.5f + RandZeroToOne(),
							0.5f + RandZeroToOne(),
							1));
		l->MakeStatic();

		pointLights.emplace_back(l);
	}

	for (int i = 0; i < spotLightCount; ++i) {
		SpotLight* l = new SpotLight();

		l->SetPosition(Vector3(	RandZeroToOne() * range.x + (int)minPos.x,
								RandZeroToOne() * range.y + (int)minPos.y,
								RandZeroToOne() * range.z + (int)minPos.z));

		l->SetDiffuseColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			1));

		l->SetRotation(Vector3(rand() % 360, 0, 0));

		l->SetRadius(75);
		l->SetAngle(20 + rand() % 45);
		spotLights.emplace_back(l);
	}
	////////////
}