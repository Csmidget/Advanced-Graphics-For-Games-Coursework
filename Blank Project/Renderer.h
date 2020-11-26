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
	void DrawLights();
	void DrawSkybox();
	void DrawNode(SceneNode* n);
	void PostProcessing();
	void CombineBuffers();
	void Blur();
	void PresentScene();

	void Resize(int x, int y) override;

	//Make a new texture
	void GenerateScreenTexture(GLuint& into, bool depth = false);

	Scene* scene;

	Mesh* quad;
	Mesh* cube;
	Mesh* sphere; //Light volume
	Shader* defaultShader;
	Shader* skyboxShader;

	Shader* pointLightShader;
	Shader* spotLightShader;


	GLuint bufferFBO;			//FBO for our G-Buffer pass
	GLuint bufferColourTex;		//Albedo goes here
	GLuint bufferNormalTex;		//Normals go here
	GLuint bufferDepthStencilTex;		//Depth goes here

	GLuint skyboxFBO;
	GLuint skyboxColourTex;

	GLuint lightingFBO;		//FBO for our lighting pass
	GLuint lightDiffuseTex;		//Store Diffuse Lighting
	GLuint lightSpecularTex;	//Store Specular Lighting

	GLuint postProcessFBO;
	GLuint postProcessColourTex[2];
	Shader* combineShader;
	Shader* blurShader;
	Shader* sceneShader;
	int outputPostProcessTex;

	Frustum frameFrustum;

	vector<SceneNode*> nodeList;
	//vector<SceneNode*> transparentNodeList;
};
