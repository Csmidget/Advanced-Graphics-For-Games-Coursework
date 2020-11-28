#include "DefaultScene.h"

#include "../nclgl/TextureManager.h"

#include "../nclgl/Camera.h"
#include "../nclgl/Window.h"
#include "../nclgl/Light.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/SceneNode.h"


const int POINT_LIGHT_NUM = 1;
const int SPOT_LIGHT_NUM = 40;

DefaultScene::DefaultScene() : Scene() {

	camera->SetPosition(Vector3(0, -150, 100));
	rotateLights = true;
	//Texture initialization
	diffuse_heightMap	=	TextureManager::LoadTexture(TEXTUREDIR"Barren Reds.JPG", SOIL_FLAG_MIPMAPS);
	normal_heightMap	=	TextureManager::LoadTexture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_FLAG_MIPMAPS);
	skybox				=	TextureManager::LoadCubemap(TEXTUREDIR"CosmicCoolCloudLeft.jpg", TEXTUREDIR"CosmicCoolCloudRight.jpg",
														TEXTUREDIR"CosmicCoolCloudTop.jpg",  TEXTUREDIR"CosmicCoolCloudBottom.jpg",
														TEXTUREDIR"CosmicCoolCloudFront.jpg", TEXTUREDIR"CosmicCoolCloudBack.jpg");

	if (!diffuse_heightMap || !normal_heightMap || !skybox) {
		return;
	}

	TextureManager::SetTextureRepeating(diffuse_heightMap, true);
	TextureManager::SetTextureRepeating(normal_heightMap, true);
	////////////


	//Shader setup//
	defaultShader =  new Shader("BufferVertex.glsl", "BufferFragment.glsl");
	bumpMapShader =  new Shader("BufferBumpVertex.glsl", "BufferBumpFragment.glsl");
	animatedShader = new Shader("SkinningVertex.glsl", "BufferFragment.glsl");
	//bumpAnimated

	if (!defaultShader->LoadSuccess() || !bumpMapShader->LoadSuccess() || !animatedShader->LoadSuccess())
		return;
	////////////////
	
	//Terrain//
	mesh_heightMap = new HeightMap(TEXTUREDIR"terraintest.png");
	SceneNode* heightMapNode = new SceneNode(mesh_heightMap);
	heightMapNode->SetBoundingRadius((mesh_heightMap->GetHeightMapSize()).Length());
	heightMapNode->SetTransform(Matrix4::Translation(Vector3(-mesh_heightMap->GetHeightMapSize().x / 2, -mesh_heightMap->GetHeightMapSize().y, -mesh_heightMap->GetHeightMapSize().z / 2)));
	heightMapNode->SetTexture(diffuse_heightMap);
	heightMapNode->SetNormal(normal_heightMap);
	heightMapNode->MakeStatic();


	heightMapNode->SetShader(bumpMapShader);
	root->AddChild(heightMapNode);
	///////////////

	//Walking man//

	mesh_roleT = Mesh::LoadFromMeshFile("Role_T.msh");
	anim_roleT = new MeshAnimation("Role_T.anm");
	mat_roleT  = new MeshMaterial("Role_T.mat");
	
	SceneNode* role_t = new SceneNode(mesh_roleT,  mat_roleT, anim_roleT, Vector4(1, 1, 1, 1), animatedShader);
	role_t->SetTransform(Matrix4::Translation(Vector3(0, -154, 0)));
	role_t->SetModelScale(Vector3(20.0f, 20.0f, 20.0f));
	role_t->SetBoundingRadius(200.0f);
	root->AddChild(role_t);

	role_t = new SceneNode(mesh_roleT, mat_roleT, anim_roleT, Vector4(1, 1, 1, 1), animatedShader);
	role_t->SetTransform(Matrix4::Translation(Vector3(10, -154, 0)));
	role_t->SetModelScale(Vector3(20.0f, 20.0f, 20.0f));
	role_t->SetBoundingRadius(200.0f);
	root->AddChild(role_t);

	//////////////
	
	//Barrel//

	mesh_Barrel = Mesh::LoadFromMeshFile("Barrel_1.msh");
	mat_Barrel = new MeshMaterial("Barrel_1.mat");
	
	const int barrelCount = 4;
	const Vector3 barrelPositions[barrelCount]{ {150,-154,150} ,{130,-154,125}, {167,-154,126}, {150,-114,135} };
	const Vector3 barrelRotations[barrelCount]{ {}			   ,{0  ,225 ,0  }, {0  ,180 ,0  }, {0  ,165  ,0 } };

	for (int i = 0; i < barrelCount; ++i) {
		SceneNode* barrel = new SceneNode(mesh_Barrel, mat_Barrel);
		barrel->SetTransform(Matrix4::Translation(barrelPositions[i]) * Matrix4::Rotation(barrelRotations[i].x, { 1,0,0 })* Matrix4::Rotation(barrelRotations[i].y, { 0,1,0 }) * Matrix4::Rotation(barrelRotations[i].z, { 0,1,0 }));
		barrel->SetModelScale(Vector3(10.0f, 10.0f, 10.0f));
		barrel->SetBoundingRadius(200.0f);
		barrel->SetShader(bumpMapShader);
		barrel->MakeStatic();
		root->AddChild(barrel);
	}

	//////////

	//Light setup
	pointLights.reserve(POINT_LIGHT_NUM);
	spotLights.reserve(SPOT_LIGHT_NUM);
	Vector3 heightmapSize = mesh_heightMap->GetHeightMapSize();

	for (int i = 0; i < POINT_LIGHT_NUM; i++)
	{
		PointLight l;

		l.SetPosition(Vector3(0, 0, 0));

	//	l.SetPosition(Vector3((rand() % (int)heightmapSize.x) - heightmapSize.x / 2,
	//		250.0f - heightmapSize.y,
	//		(rand() % (int)heightmapSize.z) - heightmapSize.x / 2));

		l.SetRadius(750);

		l.SetDiffuseColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			1));
		l.MakeStatic();
	//	l.GenerateShadowMapTexture();
		pointLights.emplace_back(l);
	}

	for (int i = 0; i < SPOT_LIGHT_NUM; ++i) {
		SpotLight l;

		l.SetPosition(Vector3((rand() % (int)heightmapSize.x) - heightmapSize.x / 2,
			250.0f - heightmapSize.y,
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
	////////////

	initialized = true;
}

DefaultScene::~DefaultScene() {

	delete defaultShader;
	delete bumpMapShader;
	delete animatedShader;
	delete bumpAnimatedShader;

	delete mesh_roleT;
	delete anim_roleT;
	delete mat_roleT;

	delete mesh_cyberSoldier;
	delete anim_cyberSoldier;
	delete mat_cyberSoldier;

	delete mesh_heightMap;

	delete mesh_Barrel;
	delete mat_Barrel;

	glDeleteTextures(1, &diffuse_heightMap);
	glDeleteTextures(1, &normal_heightMap);
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

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP)) {
		for (int i = 0; i < pointLights.size(); ++i) {
			pointLights[i].Translate(Vector3(0, 0, -1) * dt * velocity);
		}
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN)) {
		for (int i = 0; i < pointLights.size(); ++i) {
			pointLights[i].Translate(-Vector3(0, 0, -1) * dt * velocity);
		}
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT)) {
		for (int i = 0; i < pointLights.size(); ++i) {
			pointLights[i].Translate(-Vector3(1, 0, 0) * dt * velocity);
		}
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
		for (int i = 0; i < pointLights.size(); ++i) {
			pointLights[i].Translate(Vector3(1, 0, 0) * dt * velocity);
		}
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
		rotateLights = !rotateLights;
	}

	if (rotateLights) {
		for (int i = 0; i < spotLights.size(); i++) {
			spotLights[i].SetRotation(spotLights[i].GetRotation() + Vector3(1, 0, 0) * velocity * dt);
		}
	}
	camera->UpdateCamera(dt);

	Scene::Update(dt);
}