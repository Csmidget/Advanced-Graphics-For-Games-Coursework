#include "NodeTemplates.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/MeshManager.h"
#include "../nclgl/TextureManager.h"
#include "../nclgl/ShaderManager.h"

namespace Templates {
	SceneNode* Hut() {

		Shader* wallShader	= ShaderManager::LoadShader("BufferBumpVertex.glsl", "BufferBumpFragment.glsl");
		GLuint wallDiffuse	= TextureManager::LoadTexture(TEXTUREDIR"brick.tga", SOIL_FLAG_MIPMAPS);
		GLuint wallNormal	= TextureManager::LoadTexture(TEXTUREDIR"brickDOT3.tga", SOIL_FLAG_MIPMAPS);
		TextureManager::SetTextureRepeating(wallDiffuse, true);
		TextureManager::SetTextureRepeating(wallNormal, true);

		SceneNode* hut = new SceneNode();
		const int wallCount = 5;
		const Vector3 wallPositions[wallCount]{ {0.0,0,-1.5}	,{1.5,0,0.0},{-1.5,0,0.0}	,{0.0,2,-1.5}	,{0.0,2,0} };
		const Vector3 wallRotations[wallCount]{ {0,90,0}	,{0,0,0}	,{0,0,0}		,{90,90,0}	,{90,90,0} };
		Mesh* wallMesh = MeshManager::LoadMesh("Wall.msh");
		for (int i = 0; i < wallCount; i++) {
			SceneNode* wall = new SceneNode(wallMesh);
			wall->SetTexture(wallDiffuse);
			wall->SetNormal(wallNormal);
			wall->SetShader(wallShader);
			wall->SetTransform(wallPositions[i], wallRotations[i]);
			wall->SetTextureMatrix(Matrix4::Rotation(90, { 0,0,1 }));
			wall->MakeStatic();
			hut->AddChild(wall);
		}
		return hut;
	}

	SceneNode* Fence(Vector3 position = { 0,0,0 }, Vector3 rotation = { 0,0,0 }) {
		//A bit inefficient to look these up every time we create a fence but it only happens in initialization
		//and it won't create duplicates so its fine.
		Shader* defaultShader	= ShaderManager::LoadShader("BufferBumpVertex.glsl", "BufferBumpFragment.glsl");
		GLuint wallDiffuse		= TextureManager::LoadTexture(TEXTUREDIR"metall010-new-tileable.png", SOIL_FLAG_MIPMAPS);
		GLuint wallNormal		= TextureManager::LoadTexture(TEXTUREDIR"metalbump.png", SOIL_FLAG_MIPMAPS);
		TextureManager::SetTextureRepeating(wallDiffuse, true);
		TextureManager::SetTextureRepeating(wallNormal, true);

		Mesh* columnMesh = MeshManager::LoadMesh("FenceColumn.msh");
		Mesh* wallMesh = MeshManager::LoadMesh("Wall.msh");

		SceneNode* root = new SceneNode();

		SceneNode* wall = new SceneNode(wallMesh);
		wall->SetTransform({ 0,0,0 });
		wall->SetTexture(wallDiffuse);
		wall->SetNormal(wallNormal);
		wall->SetShader(defaultShader);
		root->AddChild(wall);

		SceneNode* fenceColumn1 = new SceneNode(columnMesh);
		fenceColumn1->SetTransform({ 0, 0, -1.5 }, { 0,0,0 }, { 1,0.75,1 });
		fenceColumn1->SetTexture(wallDiffuse);
		fenceColumn1->SetShader(defaultShader);
		fenceColumn1->SetNormal(wallNormal);
		root->AddChild(fenceColumn1);

		SceneNode* fenceColumn2 = new SceneNode(columnMesh);
		fenceColumn2->SetTransform({ 0, 0, 1.5 }, { 0,0,0 }, { 1,0.75,1 });
		fenceColumn2->SetTexture(wallDiffuse);
		fenceColumn2->SetShader(defaultShader);
		fenceColumn2->SetNormal(wallNormal);
		root->AddChild(fenceColumn2);

		root->SetTransform(position, rotation);
		return root;
	}

	SceneNode* WatchTower(Vector3 position = { 0,0,0 }, Vector3 rotation = { 0,0,0 }) {

		Shader* defaultShader = ShaderManager::LoadShader("BufferBumpVertex.glsl", "BufferBumpFragment.glsl");
		Mesh* ladderMesh = MeshManager::LoadMesh("Ladder.msh");
		Mesh* columnMesh = MeshManager::LoadMesh("FenceColumn.msh");
		Mesh* floorMesh = MeshManager::LoadMesh("SmallFloor.msh");
		Mesh* smallColumnMesh = MeshManager::LoadMesh("SmallColumn.msh");
		GLuint towerDiffuse = TextureManager::LoadTexture(TEXTUREDIR"metall010-new-tileable.png", SOIL_FLAG_MIPMAPS);
		GLuint towerNormal = TextureManager::LoadTexture(TEXTUREDIR"metalbump.png", SOIL_FLAG_MIPMAPS);

		SceneNode* root = new SceneNode();

		SceneNode* ladder = new SceneNode(ladderMesh);
		ladder->SetPosition({ -0.5,0,0 });
		ladder->SetTexture(towerDiffuse);
		ladder->SetNormal(towerNormal);
		ladder->SetShader(defaultShader);
		root->AddChild(ladder);

		Vector3 columnPositions[4]{ {0.5,0,0.5}, {-0.5,0,0.5}, {0.5,0,-0.5}, {-0.5,0,-0.5 } };

		for (int i = 0; i < 4; i++) {
			SceneNode* column = new SceneNode(columnMesh);
			column->SetTransform(columnPositions[i], { 0,0,0 }, { 0.5,1,0.5 });
			column->SetTexture(towerDiffuse);
			column->SetNormal(towerNormal);
			column->SetShader(defaultShader);
			root->AddChild(column);

			column = new SceneNode(smallColumnMesh);
			column->SetTransform(columnPositions[i] + Vector3(0, 3, 0), { 0,0,0 }, { 0.5,1,0.5 });
			column->SetTexture(towerDiffuse);
			column->SetNormal(towerNormal);
			column->SetShader(defaultShader);
			root->AddChild(column);

		}

		SceneNode* floor = new SceneNode(floorMesh);
		floor->SetTransform({ 0,3,0 }, { 0,0,0 }, { 1.1,1.1,1.1 });
		floor->SetTexture(towerDiffuse);
		floor->SetNormal(towerNormal);
		floor->SetShader(defaultShader);
		root->AddChild(floor);


		SceneNode* ceiling = new SceneNode(floorMesh);
		ceiling->SetTransform({ 0,4,0 }, { 0,0,0 }, { 1.1,1.1,1.1 });
		ceiling->SetTexture(towerDiffuse);
		ceiling->SetNormal(towerNormal);
		ceiling->SetShader(defaultShader);
		root->AddChild(ceiling);



		root->SetTransform(position, rotation);
		return root;
	}

	SceneNode* StreetLight(Vector3 position = { 0,0,0 }, Vector3 rotation = { 0,0,0 }) {
		Shader* defaultShader = ShaderManager::LoadShader("BufferBumpVertex.glsl", "BufferBumpFragment.glsl");

		Mesh* streetLightMesh = MeshManager::LoadMesh("StreetLamp2.msh");
		MeshMaterial* streetLightMat = MeshManager::LoadMeshMaterial("StreetLamp2.mat");

		SceneNode* streetLight = new SceneNode(streetLightMesh, streetLightMat);
		streetLight->SetShader(defaultShader);
		streetLight->SetTransform(position, rotation);
		return streetLight;
	}

	SceneNode* StreetLightDouble(Vector3 position = { 0,0,0 }, Vector3 rotation = { 0,0,0 }) {
		Shader* defaultShader = ShaderManager::LoadShader("BufferBumpVertex.glsl", "BufferBumpFragment.glsl");

		Mesh* streetLightMesh = MeshManager::LoadMesh("StreetLamp2Double.msh");
		MeshMaterial* streetLightMat = MeshManager::LoadMeshMaterial("StreetLamp2Double.mat");

		SceneNode* streetLight = new SceneNode(streetLightMesh, streetLightMat);
		streetLight->SetShader(defaultShader);
		streetLight->SetTransform(position, rotation);
		return streetLight;
	}

	SceneNode* Compound() {

		SceneNode* root = new SceneNode();

		//Hut//
		auto hut = Templates::Hut();
		hut->SetTransform({ 6.25,0,-8.75 });
		root->AddChild(hut);

		hut = Templates::Hut();
		hut->SetTransform({ 0,0,-8.75 });
		root->AddChild(hut);
		
		//Upper Wall
		root->AddChild(Fence({ -8.5,0,-13.5 }, { 0,90,0 }));
		root->AddChild(Fence({ -5.5,0,-13.5 }, { 0,90,0 }));
		root->AddChild(Fence({ -2.5,0,-13.5 }, { 0,90,0 }));
		root->AddChild(Fence({  0.5,0,-13.5 }, { 0,90,0 }));
		root->AddChild(Fence({  3.5,0,-13.5 }, { 0,90,0 }));
		root->AddChild(Fence({  6.5,0,-13.5 }, { 0,90,0 }));
		root->AddChild(Fence({  9.5,0,-13.5 }, { 0,90,0 }));

		//Left wall
		root->AddChild(Fence({ -10,0,6 }));
		root->AddChild(Fence({ -10,0,3 }));

		root->AddChild(Fence({ -10,0,-3 }));
		root->AddChild(Fence({ -10,0,-6 }));
		root->AddChild(Fence({ -10,0,-9 }));
		root->AddChild(Fence({ -10,0,-12 }));

		//Left-Lower
		root->AddChild(Fence({ -8.5,0,9 }, { 0,45,0 }));

		//Lower Wall
		root->AddChild(Fence({-5.5,0, 10.5 }, { 0,90,0 }));
		root->AddChild(Fence({-2.5,0, 10.5 }, { 0,90,0 }));
		root->AddChild(Fence({ 0.5,0, 10.5 }, { 0,90,0 }));
		root->AddChild(Fence({ 3.5,0, 10.5 }, { 0,90,0 }));
		root->AddChild(Fence({ 6.5,0, 10.5 }, { 0,90,0 }));
		root->AddChild(Fence({ 9.5,0, 10.5 }, { 0,90,0 }));

		//Right Wall
		root->AddChild(Fence({ 14,0, 6 }));
		root->AddChild(Fence({ 14,0, 3 }));
		root->AddChild(Fence({ 14,0, 0 }));
		root->AddChild(Fence({ 14,0,-3 }));
		root->AddChild(Fence({ 14,0,-6 }));
		root->AddChild(Fence({ 14,0,-9 }));

		//Lights
		root->AddChild(StreetLight({ -9.5,0,-13 }, { 0,   45, 0 }));
		root->AddChild(StreetLight({ -7  ,0, 10 }, { 0, 147.5, 0 }));
		root->AddChild(StreetLight({ 9.5  ,0, 10 }, { 0, 180, 0 }));
		root->AddChild(StreetLight({ 9.5  ,0, -13 }, { 0, 0, 0 }));
		root->AddChild(StreetLightDouble({ 2.0,0,0 }));

		root->AddChild(WatchTower({ 13,0,-1.5 }));
		root->AddChild(WatchTower({ 13,0,-1.5 }));
		root->AddChild(WatchTower({ 2.0,0,-12.5 }, { 0,90,0 }));
		root->AddChild(WatchTower({ 2.0,0,9.5 }, { 0,-90,0 }));

		return root;
	}
}