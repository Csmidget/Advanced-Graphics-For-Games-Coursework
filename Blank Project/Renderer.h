#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "nclgl\Frustum.h"
#include "nclgl\Camera.h"

class SceneNode;
class MeshAnimation;
class MeshMaterial;
class HeightMap;
class PointLight;
class SpotLight;
class Scene;
class Light;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float msec)	override;

protected:

	void BuildNodeLists(SceneNode* from, bool frustumCheck);
	void SortNodeLists();
	void ClearNodeLists();

	void DrawShadowMaps(bool staticLights);
	void DrawShadowMap(Light* light, float farPlaneDist);
	void DrawOpaques();
	void DrawTransparents();
	void DrawNode(SceneNode* n, Shader* shaderOverride = nullptr);
	void DrawNeonGrid();

	void SetupLightShader(Shader* shader);
	void DrawLights();
	void DrawSkybox();

	void PostProcessing();
	void CombineBuffers();
	void Blur();
	void PresentScene();

	void Resize(int x, int y) override;

	//Make a new texture
	void GenerateScreenTexture(GLuint& into, bool depth = false);

	Scene* scene;

	Mesh* quad;				//Screen textures
	Mesh* sphere;			//Light volume
	Shader* skyboxShader;

	Shader* pointLightShader;
	Shader* spotLightShader;
	Shader* shadowShader;

	GLuint bufferFBO;					//FBO for our G-Buffer pass
	GLuint bufferColourTex;				//Albedo goes here
	GLuint bufferNormalTex;				//Normals go here
	GLuint bufferDepthStencilTex;		//Depth goes here
	GLuint bufferTransparentColourTex;	//Transparent Albedo goes here.
	GLuint bufferTransparentNormalTex;	//Transparent Normals go here

	GLuint skyboxFBO;
	GLuint skyboxColourTex;

	GLuint lightingFBO;			//FBO for our lighting pass
	GLuint lightDiffuseTex;		//Store Diffuse Lighting
	GLuint lightSpecularTex;	//Store Specular Lighting

	Shader* neonGridShader;
	GLuint neonGridFBO;
	GLuint neonGridColourTex;

	GLuint shadowFBO;

	GLuint postProcessFBO;
	GLuint postProcessColourTex[2];
	Shader* combineShader;
	Shader* blurShader;
	Shader* basicSceneShader;
	Shader* colourCorrectionShader;
	int nextPostProcessOutput;

	bool doBlur;
	bool doNeonGrid;
	bool doColourCorrect;
	float saturationPoint;

	Frustum frameFrustum;

	vector<SceneNode*> nodeList;
	vector<SceneNode*> transparentNodeList;
};
