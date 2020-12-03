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
	void DoubleVision();
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
	int nextPostProcessOutput;

	Shader* combineShader;
	Shader* blurShader;

	Shader* doubleVisionShader;
	float doubleVisionOffset;

	Shader* basicSceneShader;
	Shader* colourCorrectionShader;

	int currentGridColourPos;
	float gridColourProgress;
	Vector4 currentGridColour;
	Vector4 gridColours[6] {Vector4(1,0,0,1),Vector4(0,1,0,1),Vector4(0,0,1,1),
							Vector4(1,1,0,1),Vector4(1,0,1,1),Vector4(0,1,1,1)};


	bool doBlur;
	bool doNeonGrid;
	bool doNeonGridColourChange;
	bool doColourCorrect;
	float saturationPoint;
	

	Frustum frameFrustum;

	vector<SceneNode*> nodeList;
	vector<SceneNode*> transparentNodeList;
};
