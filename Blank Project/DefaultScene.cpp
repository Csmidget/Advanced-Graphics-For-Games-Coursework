#include "DefaultScene.h"

#include "CameraTrack.h"
#include "NodeTemplates.h"

#include "../nclgl/TextureManager.h"
#include "../nclgl/MeshManager.h"
#include "../nclgl/ShaderManager.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Window.h"
#include "../nclgl/Light.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/SceneNode.h"
#include "HeightMapNode.h"

#include <limits>

#define POINT_LIGHT_NUM 0
#define SPOT_LIGHT_NUM 0

CameraTrack BuildTrack(Camera* cam)
{
	CameraTrack track(cam, 0.5f);

	track.AddWaypoint(Vector3(0, 0.0, 10.0), -45, 0, 0);
	track.AddWaypoint(Vector3(20.9, -12.1, -25.3), -1.73, -12.3, 0, 5.0f);
	track.AddWaypoint(Vector3(7.756, -3.112, 26.47), -31.82, -33.3, 0, 5.0f);
	track.AddWaypoint(Vector3(60.25, -14.15, 70.08), -4.6, 64.4, 0, 10.0f);

	return track;
}

DefaultScene::DefaultScene() : Scene() {

	camera->SetPosition(Vector3(0, 0.0, 10.0));
	camera->SetPitch(-45);
	rotateLight = true;
	//Texture initialization
	GLuint waterDiffuse		= TextureManager::LoadTexture(TEXTUREDIR"water.tga", SOIL_FLAG_MIPMAPS);
	GLuint waterNormal		= TextureManager::LoadTexture(TEXTUREDIR"waterbump.png", SOIL_FLAG_MIPMAPS);

	spaceSkybox  = TextureManager::LoadCubemap(	TEXTUREDIR"CosmicCoolCloudLeft.jpg", TEXTUREDIR"CosmicCoolCloudRight.jpg",
												TEXTUREDIR"CosmicCoolCloudTop.jpg", TEXTUREDIR"CosmicCoolCloudBottom.jpg",
												TEXTUREDIR"CosmicCoolCloudFront.jpg", TEXTUREDIR"CosmicCoolCloudBack.jpg");

	normalSkybox = TextureManager::LoadCubemap(	TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
												TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
												TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg");

	skybox = spaceSkybox;

	if (!skybox || !waterDiffuse || !waterNormal) {
		return;
	}

	TextureManager::SetTextureRepeating(waterDiffuse, true);
	TextureManager::SetTextureRepeating(waterNormal, true);
	////////////

	//Shader setup//
	Shader* defaultShader	= ShaderManager::LoadShader("BufferVertex.glsl", "BufferFragment.glsl");
	Shader* bumpMapShader	= ShaderManager::LoadShader("BufferBumpVertex.glsl", "BufferBumpFragment.glsl");
	Shader* animatedShader	= ShaderManager::LoadShader("SkinningVertex.glsl", "BufferFragment.glsl");
	Shader* reflectShader	= ShaderManager::LoadShader("ReflectBumpVertex.glsl", "ReflectBumpFragment.glsl");

	if (!defaultShader->LoadSuccess() || !bumpMapShader->LoadSuccess() || !animatedShader->LoadSuccess() || !reflectShader->LoadSuccess())
		return;
	////////////////
	
	//Terrain//
	//As it is custom, we cannot use the MeshManager to load the heightmap.
	heightMapMesh = new HeightMap(TEXTUREDIR"terraintest.png");
	HeightMapNode* heightMapNode = new HeightMapNode(heightMapMesh,TEXTUREDIR"terrainassign.png", TEXTUREDIR"Barren Reds.JPG", TEXTUREDIR"ground_asphalt.png", TEXTUREDIR"Barren RedsDOT3.JPG", TEXTUREDIR"AsphaltNormal.png");
	Vector3 heightmapSize = heightMapMesh->GetHeightMapSize() * 0.1f;
	heightMapNode->SetTransform(Vector3(-heightmapSize.x / 2, -heightmapSize.y, -heightmapSize.z / 2));
	heightMapNode->MakeStatic();
	heightMapNode->SetModelScale(Vector3(0.1f, 0.1f, 0.1f));
	root->AddChild(heightMapNode);
	///////////////

	//Water//
	//As with the terrain, this quad is generated in program, so we will manage it's lifecycle here.
	waterMesh = Mesh::GenerateQuad();
	water = new SceneNode(waterMesh);
	water->SetTransform(Vector3(0,-16,0), Vector3(-90, 0, 0), heightmapSize * 0.5f);
	water->SetShader(reflectShader);
	water->SetTexture(waterDiffuse);
	water->SetNormal(waterNormal);
	water->SetColour(Vector4(1, 1, 1, 0.9f));
	water->MakeStatic();
	water->SetReflective(true);
	root->AddChild(water);
	/////////

	//Soldiers//
	Mesh* roleTMesh = MeshManager::LoadMesh("Role_T.msh");
	MeshAnimation* roleTRunAnim = MeshManager::LoadMeshAnimation("Role_T.anm");
	MeshAnimation* roleTStandAnim = MeshManager::LoadMeshAnimation("Role_T_stand.anm");
	MeshMaterial* roleTMat  = MeshManager::LoadMeshMaterial("Role_T.mat");
	
	SceneNode* role_t = new SceneNode(roleTMesh,  roleTMat, roleTRunAnim, Vector4(1, 1, 1, 1), animatedShader);
	role_t->SetTransform(Vector3(0, -15.4, 0));
	role_t->SetModelScale(Vector3(2.0f, 2.0f, 2.0f));
	root->AddChild(role_t);

	role_t = new SceneNode(roleTMesh, roleTMat, roleTStandAnim, Vector4(1, 1, 1, 1), animatedShader);
	role_t->SetTransform(Vector3(25, -15.4, -37));
	role_t->SetModelScale(Vector3(2.0f, 2.0f, 2.0f));
	root->AddChild(role_t);
	//////////////



	//Compound//
	compound = Templates::Compound();
	compound->SetTransform({ 0,-15.4,0 }, { 0,0,0 }, { 4, 4, 4 });
	compound->MakeStatic();
	root->AddChild(compound);
	//////////

	//Hut Spotlight//
	spotLights.emplace_back(new SpotLight({ 25,-14.5,-30 }, { 100,0,0 }, 75.0f, 20.0f, { 1.0, 0.1, 0.1, 1.0 }));
	/////////

	//Compound Lights//
	const int pointLightCount = 6;
	Vector3 pLightPositions[pointLightCount]{ {-36.43,2.387,-50.42}, {-26.29,3.548,37.42}, {37.95,3.45,37.01},{37.95,3.477,-48.96},{7.964,3.734,-3.472},{7.964,3.734,3.714} };
	Vector4 pLightColours[pointLightCount]{ {1,0,0,1},{0,1,0,1},{0,0,1,1},{1,0,1,1},{1,1,1,1},{1,1,1,1}  };
	bool	pIsStatic[pointLightCount]{ true,true,true,true,false,false };
	for (int i = 0; i < pointLightCount; i++) {
		PointLight* l = new PointLight(pLightPositions[i],pLightColours[i],pLightColours[i],75.0f);
		pIsStatic[i] ? l->MakeStatic() : l->MakeDynamic();
		pointLights.push_back(l);
	}
	/////////////////
	
	//Barrels//
	SceneNode* barrelStack = Templates::BarrelStack();
	barrelStack->SetPosition({ 13,-11.4, 12.5 });
	root->AddChild(barrelStack);
	//////////
	spinningLight = new SpotLight({ 2, 80, 0 }, { 50, 0, 0 }, 500.0f, 15.0f);
	spotLights.emplace_back(spinningLight);
	Vector3 halfSize = { heightmapSize / 2 };
	Scene::GenerateRandomLights(POINT_LIGHT_NUM, SPOT_LIGHT_NUM, { -halfSize.x,50,-halfSize.z }, { halfSize.x,50,halfSize.z });

	waterRotate = 0.0f;
	waterCycle = 0.0f;

	track = new CameraTrack{BuildTrack(camera)};


	initialized = true;
}

DefaultScene::~DefaultScene() {

	delete track;

	delete waterMesh;
	delete heightMapMesh;
}

void DefaultScene::Update(float dt) {
	camera->Rotate(-Window::GetMouse()->GetRelativePosition().y, -Window::GetMouse()->GetRelativePosition().x, 0);

	Matrix4 rotation = Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)) * Matrix4::Rotation(camera->GetRoll(), Vector3(0, 0, 1));

	Vector3 forward = rotation * Vector3(0, 0, -1) * dt;
	Vector3 right = rotation * Vector3(1, 0, 0) * dt;
	Vector3 up = Vector3(0, 1, 0);
	Vector3 velocity = Vector3(45, 45, 45);

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

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_Q)) {
		compound->rotate({ 0,45 * dt,0 });
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_E)) {
		compound->rotate({ 0,-45 * dt,0 });
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP)) {
		compound->Translate({0 ,0,-30*dt });
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN)) {
		compound->Translate({ 0 ,0,30 * dt });
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT)) {
		compound->Translate({ 30 * dt ,0,0 });

	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
		compound->Translate({ -30 * dt ,0,0 });
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_9)) {
		rotateLight = !rotateLight;
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_8)) {
		skybox = skybox == spaceSkybox ? normalSkybox : spaceSkybox;
	}

	if (rotateLight) {
		spinningLight->Rotate({ 0,10 * dt, 0 });
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P)) {
		Vector3 pos = camera->GetPosition();
		Vector3 rot = { camera->GetPitch(), camera->GetYaw(), camera->GetRoll() };
		std::cout.precision(4);
		std::cout << "Camera pos: <" << pos.x << "," << pos.y << "," << pos.z << "> <" << rot.x << "," << rot.y << "," << rot.z << ">\n";
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_0)) {
		track->Start();
	}

	waterRotate += dt * 2.0f; //2 degrees a second
	waterCycle += dt * 0.05f; //10 units a second

//	spinningLight->Rotate()

	water->SetTextureMatrix(
		Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
		Matrix4::Scale(Vector3(10, 10, 10)) *
		Matrix4::Rotation(waterRotate, Vector3(0, 0, 1)));

	track->Update(dt);

	camera->UpdateCamera(dt);
	Scene::Update(dt);
}