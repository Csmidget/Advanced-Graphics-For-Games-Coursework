#include "DefaultScene.h"

#include "NodePrefabs.h"
#include "TrackPrefabs.h"

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

//These are set to 0, but you can add randomly positioned lights to the scene here.
//Be careful, any added lights will have shadow maps generated, too many may crash the application.
//My computer supports up to ~40 before seeing performance issues.
#define POINT_LIGHT_NUM 0
#define SPOT_LIGHT_NUM 0

DefaultScene::DefaultScene() : Scene() {

	rotateLight = true;
	waterRotate = 0.0f;
	waterCycle = 0.0f;


	//##### Camera #####
	camera->SetPosition(Vector3(0, 0.0, 10.0));
	camera->SetRotation({ -45,0,0 });

	track = Prefabs::BuildCameraTrack(camera);
	track->Start();
	//##################


	//##### Skybox #####
	skybox  = TextureManager::LoadCubemap(	TEXTUREDIR"CosmicCoolCloudLeft.jpg", TEXTUREDIR"CosmicCoolCloudRight.jpg",
												TEXTUREDIR"CosmicCoolCloudTop.jpg", TEXTUREDIR"CosmicCoolCloudBottom.jpg",
												TEXTUREDIR"CosmicCoolCloudFront.jpg", TEXTUREDIR"CosmicCoolCloudBack.jpg");
	if (!skybox) 
		return;
	//##################


	//##### Shaders #####
	Shader* animatedShader	= ShaderManager::LoadShader("SkinningVertex.glsl", "BufferFragment.glsl");
	Shader* reflectShader	= ShaderManager::LoadShader("ReflectBumpVertex.glsl", "ReflectBumpFragment.glsl");

	if (!animatedShader->LoadSuccess() || !reflectShader->LoadSuccess())
		return;
	//###################
	

	//##### Terrain #####
	//As it is a custom mesh, we cannot use the MeshManager to load the heightmap.
	heightMapMesh = new HeightMap(TEXTUREDIR"terraintest.png");
	HeightMapNode* heightMapNode = new HeightMapNode(heightMapMesh,TEXTUREDIR"terrainassign.png", TEXTUREDIR"Barren Reds.JPG", TEXTUREDIR"ground_asphalt.png", TEXTUREDIR"Barren RedsDOT3.JPG", TEXTUREDIR"AsphaltNormal.png");
	Vector3 heightmapSize = heightMapMesh->GetHeightMapSize() * 0.1f;
	heightMapNode->SetTransform(Vector3(-heightmapSize.x / 2, -heightmapSize.y, -heightmapSize.z / 2));
	heightMapNode->MakeStatic();
	heightMapNode->SetModelScale(Vector3(0.1f, 0.1f, 0.1f));
	root->AddChild(heightMapNode);
	//################### 


	//##### Water #####
	//As with the terrain, this quad is generated in program, so we will manage it's lifecycle here.
	GLuint waterDiffuse = TextureManager::LoadTexture(TEXTUREDIR"water.tga", SOIL_FLAG_MIPMAPS);
	GLuint waterNormal = TextureManager::LoadTexture(TEXTUREDIR"waterbump.png", SOIL_FLAG_MIPMAPS);
	TextureManager::SetTextureRepeating(waterDiffuse, true);
	TextureManager::SetTextureRepeating(waterNormal, true);
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
	//#################


	//##### Moving Soldiers #####
	Mesh* roleTMesh = MeshManager::LoadMesh("Role_T.msh");
	MeshAnimation* roleTRunAnim = MeshManager::LoadMeshAnimation("Role_T.anm");
	MeshAnimation* roleTStandAnim = MeshManager::LoadMeshAnimation("Role_T_stand.anm");
	MeshMaterial* roleTMat  = MeshManager::LoadMeshMaterial("Role_T.mat");
	
	SceneNode* role_t = new SceneNode(roleTMesh,  roleTMat, roleTRunAnim, Vector4(1, 1, 1, 1), animatedShader);
	role_t->SetTransform(Vector3(0.0f, -15.4f, 0.0f));
	role_t->SetModelScale(Vector3(2.0f, 2.0f, 2.0f));
	patrols.push_back(Prefabs::BuildCompoundPatrol(role_t));
	root->AddChild(role_t);

	role_t = new SceneNode(roleTMesh, roleTMat, roleTRunAnim, Vector4(1, 1, 1, 1), animatedShader);
	role_t->SetTransform(Vector3(0.0f, -15.4f, 0.0f));
	role_t->SetModelScale(Vector3(2.0f, 2.0f, 2.0f));
	patrols.push_back(Prefabs::BuildBottomRightPatrol(role_t));
	root->AddChild(role_t);

	role_t = new SceneNode(roleTMesh, roleTMat, roleTRunAnim, Vector4(1, 1, 1, 1), animatedShader);
	role_t->SetTransform(Vector3(0.0f, -15.4f, 0.0f));
	role_t->SetModelScale(Vector3(2.0f, 2.0f, 2.0f));
	patrols.push_back(Prefabs::BuildLeftPatrol(role_t));
	root->AddChild(role_t);

	role_t = new SceneNode(roleTMesh, roleTMat, roleTRunAnim, Vector4(1, 1, 1, 1), animatedShader);
	role_t->SetTransform(Vector3(0.0f, -15.4f, 0.0f));
	role_t->SetModelScale(Vector3(2.0f, 2.0f, 2.0f));
	patrols.push_back(Prefabs::BuildTopRightPatrol(role_t));
	root->AddChild(role_t);
	//###########################


	//##### Stationary Soldiers #####
	root->AddChild(Prefabs::CompoundStationarySoldiers());
	//###############################


	//##### Compound #####
	SceneNode* compound = Prefabs::Compound();
	compound->SetTransform(Vector3(0.0f,-15.4f,0.0f), Vector3(0.0f,0.0f,0.0f), Vector3(4.0f, 4.0f, 4.0f));
	compound->MakeStatic();
	root->AddChild(compound);

	//Hut Spotlight
	spotLights.emplace_back(new SpotLight(Vector3(20.0f,-14.5f,-32.0f), Vector3(100.0f,0.0f,0.0f), 75.0f, 20.0f, Vector4(1.0f, 0.1f, 0.1f, 1.0f)));
	//####################


	//##### Barrels #####
	SceneNode* barrelStack = Prefabs::BarrelStack();
	barrelStack->SetPosition(Vector3( 15.0f,-11.4f, 14.5f ));
	barrelStack->MakeStatic();
	root->AddChild(barrelStack);

	barrelStack = Prefabs::BarrelStack();
	barrelStack->SetPosition(Vector3(-35.0f,-11.4f, -45.0f ));
	barrelStack->MakeStatic();
	root->AddChild(barrelStack);

	barrelStack = Prefabs::BarrelStack();
	barrelStack->SetPosition(Vector3(-28.0f,-11.4f, 35.0f ));
	barrelStack->MakeStatic();
	root->AddChild(barrelStack);
	//###################


	//##### Pond #####
	SceneNode* pondLampPost = Prefabs::StreetLight(Vector3( -160.0f,-15.4f,-100.0f ), Vector3( 0.0f,-90.0f,0.0f ));
	pondLampPost->Scale(Vector3(4.0f,4.0f,4.0f));
	pondLampPost->MakeStatic();
	root->AddChild(pondLampPost);
	PointLight* pondLight = new PointLight(Vector3(-164.4f,8.443f,-100.1f ), Vector4(1.0f,1.0f,1.0f,1.0f ), Vector4(1.0f,1.0f,1.0f,1.0f), 100.0f);
	pondLight->MakeStatic();
	pointLights.emplace_back(pondLight);
	//################


	//##### Compound PointLights #####
	const int pointLightCount = 6;
	Vector3 pLightPositions[pointLightCount]{	{-36.43f,2.387f,-50.42f},	{-26.29f,3.548f,37.42f},	{37.95f,3.45f,37.01f},
												{37.95f,3.477f,-48.96f},	{7.964f,3.734f,-3.472f},	{7.964f,3.734f,3.714f},
												 };

	Vector4 pLightColours[pointLightCount]{		{2.0f,0.0f,0.0f,1.0f},		{0.0f,2.0f,0.0f,1.0f},		{0.0f,0.0f,2.0f,1.0f},
												{2.0f,0.0f,2.0f,1.0f},		{1.0f,1.0f,1.0f,1.0f},		{1.0f,1.0f,1.0f,1.0f}};

	bool	pIsStatic[pointLightCount]{ true,true,true,true,false,false };
	for (int i = 0; i < pointLightCount; i++) {
		PointLight* l = new PointLight(pLightPositions[i],pLightColours[i],pLightColours[i],75.0f);
		pIsStatic[i] ? l->MakeStatic() : l->MakeDynamic();
		pointLights.push_back(l);
	}
	//################################


	//##### Central Spinning Spotlight ######
	spinningLight = new SpotLight(Vector3( 2.0f, 80.0f, 0.0f ), Vector3(50.0f, 0.0f, 0.0f), 500.0f, 15.0f);
	spotLights.emplace_back(spinningLight);
	//#######################################


	//##### Camera Light #####
	cameraLight = new SpotLight(Vector3(0.0f,0.0f,0.0f), Vector3(0.0f,0.0f,0.0f), 300.0f, 30.0f, Vector4(2.0f,2.0f,2.0f,2.0f));
	cameraLight->SetActive(false);
	spotLights.emplace_back(cameraLight);
	//########################


	//##### Randomly Generated Lights #####
	//If POINT_LIGHT_NUM or SPOT_LIGHT_NUM are greater than 0, generate that number of lights randomly in the scene.
	Vector3 halfSize = { (heightmapSize / 2) - Vector3(20.0f,0.0f,20.0f) };
	Scene::GenerateRandomLights(POINT_LIGHT_NUM, SPOT_LIGHT_NUM, { -halfSize.x,50,-halfSize.z }, { halfSize.x,50,halfSize.z });
	//#####################################

	initialized = true;
}

DefaultScene::~DefaultScene() {

	delete track;

	delete heightMapMesh;

	for (auto p : patrols) {
		delete p;
	}

	delete waterMesh;
	delete heightMapMesh;
}

void DefaultScene::Update(float dt) {

	//##### Camera Controls ######
	camera->Rotate({ -Window::GetMouse()->GetRelativePosition().y, -Window::GetMouse()->GetRelativePosition().x, 0 });

	Vector3 rotVec = camera->GetRotation();
	Matrix4 rotation = Matrix4::Rotation(rotVec.y, Vector3(0, 1, 0)) * Matrix4::Rotation(rotVec.z, Vector3(0, 0, 1));

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

	camera->UpdateCamera(dt);
	//############################

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P)) {
		Vector3 pos = camera->GetPosition();
		Vector3 rot = camera->GetRotation();
		std::cout.precision(4);
		std::cout << "Camera pos: <" << pos.x << "," << pos.y << "," << pos.z << "> <" << rot.x << "," << rot.y << "," << rot.z << ">\n";
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_L)) {
		cameraLight->SetActive(!cameraLight->GetActive());
	}
	cameraLight->SetPosition(camera->GetPosition());
	cameraLight->SetRotation(camera->GetRotation() + Vector3(90, 0, 0));

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_5)) {
		rotateLight = !rotateLight;
	}
	if (rotateLight) {
		spinningLight->Rotate({ 0,10 * dt, 0 });
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_CONTROL)) {
		track->IsActive() ? track->Stop() : track->Start();
	}

	waterRotate += dt * 1.0f; //1 degree a second
	waterCycle += dt * 0.025f;

	water->SetTextureMatrix(
		Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
		Matrix4::Scale(Vector3(10, 10, 10)) *
		Matrix4::Rotation(waterRotate, Vector3(0, 0, 1)));


	track->Update(dt);
	for (auto t : patrols) {
		t->Update(dt);
	}

	Scene::Update(dt);
}