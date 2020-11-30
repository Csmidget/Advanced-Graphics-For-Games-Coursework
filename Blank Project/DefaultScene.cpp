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

#include <limits>

const int POINT_LIGHT_NUM = 0;
const int SPOT_LIGHT_NUM = 0;

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
	rotateLights = true;
	//Texture initialization
	GLuint heightMapDiffuse	= TextureManager::LoadTexture(TEXTUREDIR"Barren Reds.JPG", SOIL_FLAG_MIPMAPS);
	GLuint heightMapNormal	= TextureManager::LoadTexture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_FLAG_MIPMAPS);
	GLuint waterDiffuse		= TextureManager::LoadTexture(TEXTUREDIR"water.tga", SOIL_FLAG_MIPMAPS);
	GLuint waterNormal		= TextureManager::LoadTexture(TEXTUREDIR"waterbump.png", SOIL_FLAG_MIPMAPS);

	spaceSkybox  = TextureManager::LoadCubemap(	TEXTUREDIR"CosmicCoolCloudLeft.jpg", TEXTUREDIR"CosmicCoolCloudRight.jpg",
												TEXTUREDIR"CosmicCoolCloudTop.jpg", TEXTUREDIR"CosmicCoolCloudBottom.jpg",
												TEXTUREDIR"CosmicCoolCloudFront.jpg", TEXTUREDIR"CosmicCoolCloudBack.jpg");

	normalSkybox = TextureManager::LoadCubemap(	TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
												TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
												TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg");

	skybox = spaceSkybox;

	if (!heightMapDiffuse || !heightMapNormal || !skybox) {
		return;
	}

	TextureManager::SetTextureRepeating(heightMapDiffuse, true);
	TextureManager::SetTextureRepeating(heightMapNormal, true);
	TextureManager::SetTextureRepeating(heightMapNormal, true);
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
	SceneNode* heightMapNode = new SceneNode(heightMapMesh);
	Vector3 heightmapSize = heightMapMesh->GetHeightMapSize() * 0.1f;
	heightMapNode->SetTransform(Vector3(-heightmapSize.x / 2, -heightmapSize.y, -heightmapSize.z / 2));
	heightMapNode->SetTexture(heightMapDiffuse);
	heightMapNode->SetNormal(heightMapNormal);
	heightMapNode->MakeStatic();
	heightMapNode->SetModelScale(Vector3(0.1f, 0.1f, 0.1f));
	heightMapNode->SetShader(bumpMapShader);
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

	//Walking man//
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

	//Hut Spotlight
	SpotLight l;
	l.SetPosition({ 25,-14.5,-30 });
	l.SetRadius(75.0f);
	l.SetAngle(20);
	l.SetDiffuseColour({ 1.0, 0.1, 0.1, 1.0});
	l.SetRotation({ 100,0,0 });
	l.SetDirection(Vector3(0, -1, 0));
	spotLights.emplace_back(l);
	/////////

	//Compound//
	compound = Templates::Compound();
	compound->SetTransform({ 0,-15.4,0 }, { 0,0,0 }, { 4, 4, 4 });
	compound->MakeStatic();
	root->AddChild(compound);
	//////////

	//Compound Lights//
	const int pointLightCount = 6;
	Vector3 pLightPositions[pointLightCount]{ {-36.43,2.387,-50.42}, {-26.29,3.548,37.42}, {37.95,3.45,37.01},{37.95,3.477,-48.96},{7.964,3.734,-3.472},{7.964,3.734,3.714} };
	Vector4 pLightColours[pointLightCount]{ {1,0.4,0.4,1},{0.4,1,0.4,1},{0.4,0.4,1,1},{1,0.4,1,1} };
	bool	pIsStatic[pointLightCount]{ true,true,true,true,false,false };
	for (int i = 0; i < pointLightCount; i++) {
		PointLight l;
		l.SetPosition(pLightPositions[i]);
		l.SetDiffuseColour(pLightColours[i]);
		l.SetRadius(75.0f);
		pIsStatic[i] ? l.MakeStatic() : l.MakeDynamic();
		pointLights.push_back(l);
	}



	/////////////////
	
	//Barrel//
	Mesh* barrelMesh = MeshManager::LoadMesh("Barrel_1.msh");
	MeshMaterial* barrelMat = MeshManager::LoadMeshMaterial("Barrel_1.mat");
	
	const int barrelCount = 4;
	const Vector3 barrelPositions[barrelCount]{ {15.0,-15.4,15.0} ,{13.0,-15.4,12.5}, {16.7,-15.4,12.6}, {15.0,-11.4,13.5} };
	const Vector3 barrelRotations[barrelCount]{ {}			   ,{0  ,22.5 ,0  }, {0  ,18.0 ,0  }, {0  ,16.5  ,0 } };

	for (int i = 0; i < barrelCount; ++i) {
		SceneNode* barrel = new SceneNode(barrelMesh, barrelMat);
		barrel->SetTransform(barrelPositions[i], barrelRotations[i]);
		barrel->SetShader(bumpMapShader);
		barrel->MakeStatic();
		root->AddChild(barrel);
	}
	//////////

	//PointLight centralLight;
	//centralLight.SetPosition({ 0, 10, 0 });
	//centralLight.SetRadius(100.0f);
	//centralLight.MakeStatic();
	//pointLights.emplace_back(centralLight);

	//Light setup
	pointLights.reserve(POINT_LIGHT_NUM);
	spotLights.reserve(SPOT_LIGHT_NUM);

	for (int i = 0; i < POINT_LIGHT_NUM; i++)
	{
		PointLight l;

		l.SetPosition(Vector3((rand() % (int)heightmapSize.x) - heightmapSize.x / 2,
			25.0f ,
			(rand() % (int)heightmapSize.z) - heightmapSize.x / 2));

		l.SetRadius(75.0);

		l.SetDiffuseColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			1));
		l.MakeStatic();

		pointLights.emplace_back(l);
	}

	for (int i = 0; i < SPOT_LIGHT_NUM; ++i) {
		SpotLight l;

		l.SetPosition(Vector3((rand() % (int)heightmapSize.x) - heightmapSize.x / 2,
			25.0f,
			(rand() % (int)heightmapSize.z) - heightmapSize.x / 2));

		l.SetDiffuseColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			1));

		l.SetRotation(Vector3(rand() % 360, 0, 0));

		l.SetRadius(75);
		l.SetAngle(20 + rand() % 45);
		l.SetDirection(Vector3(0, -1, 0));
		spotLights.emplace_back(l);
	}
	////////////

	waterRotate = 0.0f;
	waterCycle = 0.0f;

	track = new CameraTrack{BuildTrack(camera)};
	//track->Start();

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
		rotateLights = !rotateLights;
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_8)) {
		skybox = skybox == spaceSkybox ? normalSkybox : spaceSkybox;
	}

	if (rotateLights) {
		for (int i = 1; i < spotLights.size(); i++) {
			spotLights[i].SetRotation(spotLights[i].GetRotation() + Vector3(1, 0, 0) * velocity * dt);
		}
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

	water->SetTextureMatrix(
		Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
		Matrix4::Scale(Vector3(10, 10, 10)) *
		Matrix4::Rotation(waterRotate, Vector3(0, 0, 1)));

	track->Update(dt);

	camera->UpdateCamera(dt);
	Scene::Update(dt);
}