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
		const Vector3 wallPositions[wallCount]{ {0.0,1,-2}	,{2.0,1,0.0},{-2.0,1,0.0}	,{0.0,2,-1}	,{0.0,2,1} };
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

	SceneNode* Prison() {

		Shader* defaultShader = ShaderManager::LoadShader("BufferVertex.glsl", "BufferFragment.glsl");
		GLuint wallTexture = TextureManager::LoadTexture("rusted_down.jpg");
		Mesh* columnMesh = MeshManager::LoadMesh("FenceColumn.msh");


		SceneNode* root = new SceneNode();


		SceneNode* fenceColumn = new SceneNode(columnMesh);
		fenceColumn->SetTexture(wallTexture);
		fenceColumn->SetShader(defaultShader);

		root->AddChild(fenceColumn);
		return root;
	}
}