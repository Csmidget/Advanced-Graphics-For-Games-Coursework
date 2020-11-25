#include "DefaultScene.h"

#include "../nclgl/Camera.h"
#include "../nclgl/Window.h"
#include "../nclgl/Light.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/SceneNode.h"

const int POINT_LIGHT_NUM = 24;
const int SPOT_LIGHT_NUM = 40;

DefaultScene::DefaultScene() : Scene() {

	//Texture initialization
	heightMapTexture = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	heightMapBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	if (!heightMapTexture || !heightMapBump) {
		return;
	}


	//Terrain
	mesh_heightMap = new HeightMap(TEXTUREDIR"noise.png");
	SceneNode* heightMapNode = new SceneNode(mesh_heightMap);
	heightMapNode->SetBoundingRadius((mesh_heightMap->GetHeightMapSize()).Length());
	heightMapNode->SetTransform(Matrix4::Translation(Vector3(-mesh_heightMap->GetHeightMapSize().x / 2, -mesh_heightMap->GetHeightMapSize().y, -mesh_heightMap->GetHeightMapSize().z / 2)));
	heightMapNode->SetTexture(heightMapTexture);
	heightMapNode->SetNormal(heightMapBump);

	heightMapShader = new Shader("BumpVertex.glsl", "BufferFragment.glsl");
	if (!heightMapShader->LoadSuccess())
		return;

	heightMapNode->SetShader(heightMapShader);
	root->AddChild(heightMapNode);


	//Walking man
	mesh_roleT =	Mesh::LoadFromMeshFile("Role_T.msh");
	anim_roleT =	new MeshAnimation("Role_T.anm");
	anim_roleT->GenerateRelativeJoints(mesh_roleT->GetInverseBindPose());
	mat_roleT =		new MeshMaterial("Role_T.mat");
	shader_roleT =	new Shader("SkinningBumpVertex.glsl", "BufferFragment.glsl");

	if (!shader_roleT->LoadSuccess())
		return;

	SceneNode* role_t = new SceneNode(mesh_roleT, anim_roleT, mat_roleT, Vector4(1, 1, 1, 1), shader_roleT);
	role_t->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
	role_t->SetBoundingRadius(200.0f);
	root->AddChild(role_t);


	//Light setup
	pointLights.reserve(POINT_LIGHT_NUM);
	spotLights.reserve(SPOT_LIGHT_NUM);
	Vector3 heightmapSize = mesh_heightMap->GetHeightMapSize();

	for (int i = 0; i < POINT_LIGHT_NUM; i++)
	{
		Light l;

		l.SetPosition(Vector3((rand() % (int)heightmapSize.x) - heightmapSize.x / 2,
			250.0f,
			(rand() % (int)heightmapSize.z) - heightmapSize.x / 2));

		l.SetRadius(500 + (rand() % 250));

		l.SetDiffuseColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			1));
		pointLights.emplace_back(l);
	}

	for (int i = 0; i < SPOT_LIGHT_NUM; ++i) {
		SpotLight l;

		l.SetPosition(Vector3((rand() % (int)heightmapSize.x) - heightmapSize.x / 2,
			250.0f,
			(rand() % (int)heightmapSize.z) - heightmapSize.x / 2));

		l.SetDiffuseColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			1));

		l.SetRotation(Vector3(rand() % 360, 0, 0));

		l.SetRadius(500 + (rand() % 250));
		l.SetAngle(20 + rand() % 45);
		l.SetDirection(Vector3(0, -1, 0));
		spotLights.emplace_back(l);
	}

	initialized = true;
}

DefaultScene::~DefaultScene() {
	delete mesh_roleT;
	delete anim_roleT;
	delete mat_roleT;
	delete shader_roleT;

	delete mesh_heightMap;
	delete heightMapShader;

	glDeleteTextures(1, &heightMapTexture);
	glDeleteTextures(1, &heightMapBump);
}

void DefaultScene::Update(float dt) {
	camera->Rotate(-Window::GetMouse()->GetRelativePosition().y, -Window::GetMouse()->GetRelativePosition().x, 0);

	Matrix4 rotation = Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)) * Matrix4::Rotation(camera->GetRoll(), Vector3(0, 0, 1));

	Vector3 forward = rotation * Vector3(0, 0, -1) * dt;
	Vector3 right = rotation * Vector3(1, 0, 0) * dt;
	Vector3 up = Vector3(0, 1, 0);
	Vector3 velocity = Vector3(90, 90, 90);

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		camera->Translate(forward * velocity);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		camera->Translate(-forward * velocity);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		camera->Translate(-right * velocity);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		camera->Translate(right * velocity);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		camera->Translate(up * velocity * dt);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		camera->Translate(-up * velocity.y * dt);
	}

	for (int i = 0; i < spotLights.size(); i++) {
		spotLights[i].SetRotation(spotLights[i].GetRotation() + Vector3(1, 0, 0) * velocity * dt);
	}

	camera->UpdateCamera(dt);
}