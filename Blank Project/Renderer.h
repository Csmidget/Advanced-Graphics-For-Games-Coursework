#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "nclgl\Frustum.h"
#include "nclgl\Camera.h"

class SceneNode;
class MeshAnimation;
class MeshMaterial;
class HeightMap;
class Light;
class SpotLight;
class Scene;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float msec)	override;

protected:

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawOpaques();
	void DrawTransparents();
	void DrawLights();
	void DrawNode(SceneNode* n);

	void CombineBuffers();
	void Resize(int x, int y) override;

	//Make a new texture
	void GenerateScreenTexture(GLuint& into, bool depth = false);

	Scene* scene;

	Mesh* quad;
	Mesh* cube;
	Mesh* sphere; //Light volume
	Shader* shader;
	GLuint texture;

	Mesh* mesh_roleT;
	MeshAnimation* anim_roleT;
	MeshMaterial* mat_roleT;
	Shader* shader_roleT;

	HeightMap* mesh_heightMap;
	Shader* heightMapShader;	
	GLuint heightMapTexture;
	GLuint heightMapBump;

	Shader* pointLightShader;
	Shader* spotLightShader;
	Shader* combineShader;

	GLuint bufferFBO;			//FBO for our G-Buffer pass
	GLuint bufferColourTex;		//Albedo goes here
	GLuint bufferNormalTex;		//Normals go here
	GLuint bufferDepthTex;		//Depth goes here

	GLuint lightingFBO;		//FBO for our lighting pass
	GLuint lightDiffuseTex;		//Store Diffuse Lighting
	GLuint lightSpecularTex;	//Store Specular Lighting

	Frustum frameFrustum;

	vector<SceneNode*> nodeList;
	vector<SceneNode*> transparentNodeList;

	Light* pointLights;			//Array of point lighting data
	SpotLight* spotLights;		//Array of spot lighting data
};
