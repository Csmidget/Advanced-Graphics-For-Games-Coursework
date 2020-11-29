#include "Renderer.h"
#include "DefaultScene.h"

#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"
#include "../nclgl/TextureManager.h"
#include "../nclgl/MeshManager.h"

#include <algorithm>

const int BLUR_PASSES = 10;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	
	scene = new DefaultScene();

	if (!scene->initialized) {
		return;
	}
	//Run a single update on scene to initialize world transforms etc.
	scene->Update(0);

	quad = Mesh::GenerateQuad();
	//The mesh manager will handle the lifecycle of this, so we won't need to delete them in the renderer.
	sphere = MeshManager::LoadMesh("Sphere.msh");

	doBlur = false;

	//Default all of our textures to 0. This is so that GenerateSceenTexture() recognises
	//they are empty and generates a new texture for them.
	bufferDepthStencilTex = 0;
	bufferColourTex = 0;
	bufferNormalTex = 0;
	lightDiffuseTex = 0;
	lightSpecularTex = 0;
	skyboxColourTex = 0;
	bufferTransparentColourTex = 0;
	bufferTransparentNormalTex = 0;
	outputPostProcessTex = 0;
	postProcessColourTex[0] = 0;
	postProcessColourTex[1] = 0;

	projMatrix = scene->GetCameraPerspective(width, height);

	skyboxShader = new Shader("SkyboxVertex.glsl", "SkyboxFragment.glsl");
	blurShader = new Shader("TexturedVertex.glsl", "ProcessFragment.glsl");
	sceneShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	pointLightShader = new Shader("PointLightVertex.glsl", "PointLightShadowFragment.glsl");
	spotLightShader = new Shader("PointLightVertex.glsl", "SpotLightShadowFragment.glsl");
	shadowShader = new Shader("ShadowVert.glsl", "ShadowFrag.glsl", "ShadowGeom.glsl");
	combineShader = new Shader("CombineVertex.glsl", "CombineFragment.glsl");

	if (!pointLightShader->LoadSuccess()|| !spotLightShader->LoadSuccess()	||	!combineShader->LoadSuccess()	||
		!skyboxShader->LoadSuccess()	|| !blurShader->LoadSuccess()		||	!sceneShader->LoadSuccess()		||
		!shadowShader->LoadSuccess())
		return;

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &lightingFBO);
	glGenFramebuffers(1, &skyboxFBO);
	glGenFramebuffers(1, &postProcessFBO);
	glGenFramebuffers(1, &shadowFBO);

	GLenum buffers[2] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1
	};

	//Generate our scene depth texture...
	GenerateScreenTexture(bufferDepthStencilTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(bufferTransparentColourTex);
	GenerateScreenTexture(bufferTransparentNormalTex);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);
	GenerateScreenTexture(skyboxColourTex);
	GenerateScreenTexture(postProcessColourTex[0]);
	GenerateScreenTexture(postProcessColourTex[1]);


	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}


	glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}


	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, skyboxColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessColourTex[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	BuildNodeLists(scene->root, false);
	DrawShadowMaps(true);
	ClearNodeLists();

	init = true;
}

Renderer::~Renderer(void) {

	delete scene;
	delete quad;

	delete pointLightShader;
	delete spotLightShader;
	delete combineShader;
	delete skyboxShader;
	delete blurShader;
	delete sceneShader;

	glDeleteTextures(1, &bufferDepthStencilTex);
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteTextures(1, &skyboxColourTex);
	glDeleteTextures(2, postProcessColourTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &lightingFBO);
	glDeleteFramebuffers(1, &skyboxFBO);
	glDeleteFramebuffers(1, &postProcessFBO);
}

void Renderer::GenerateScreenTexture(GLuint& into, bool depth) {
	
	//Generate a new gl texture if a pointer isn't already provided.
	if (!into) {
		glGenTextures(1, &into);
	}

	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint internalformat = depth ? GL_DEPTH24_STENCIL8 : GL_RGBA8;
	GLuint format = depth ? GL_DEPTH_STENCIL : GL_RGBA;
	GLuint type = depth ? GL_UNSIGNED_INT_24_8 : GL_UNSIGNED_BYTE;

	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::Resize(int x, int y) {
	OGLRenderer::Resize(x, y);

	//Generate our scene depth texture...
	GenerateScreenTexture(bufferDepthStencilTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(bufferTransparentColourTex);
	GenerateScreenTexture(bufferTransparentNormalTex);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);
	GenerateScreenTexture(skyboxColourTex);
	GenerateScreenTexture(postProcessColourTex[0]);
	GenerateScreenTexture(postProcessColourTex[1]);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);

	//Reuse the same depth/stencil texture for the skybox.
	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, skyboxColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::UpdateScene(float dt) {

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
		doBlur = !doBlur;
	}

	scene->Update(dt);

	viewMatrix = scene->camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
}

void Renderer::BuildNodeLists(SceneNode* from, bool frustumCheck) {
	if (!frustumCheck || frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - scene->camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}

	for (auto i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists(*i, frustumCheck);
	}
}

void Renderer::DrawShadowMaps(bool staticLights) {

	int resolution = staticLights ? STATIC_SHADOW_RESOLUTION : DYNAMIC_SHADOW_RESOLUTION;
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glViewport(0, 0, STATIC_SHADOW_RESOLUTION, STATIC_SHADOW_RESOLUTION);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glClearColor(0, 0, 0, 0);

	BindShader(shadowShader);

	for (auto& l : scene->pointLights) {
		if (!staticLights || l.IsStatic())
			DrawShadowMap(STATIC_SHADOW_RESOLUTION, l, l.GetRadius());
	}

	for (auto& l : scene->spotLights) {
		if (!staticLights || l.IsStatic())
			DrawShadowMap(STATIC_SHADOW_RESOLUTION, l, l.GetRadius());
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);

}

void Renderer::DrawShadowMap(int resolution,Light& light,float farPlaneDist) {

	if (light.GetShadowMap() == 0)
		light.GenerateShadowMapTexture();

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, light.GetShadowMap(), 0);

	glClear(GL_DEPTH_BUFFER_BIT);

	Matrix4 lightProjMatrix = Matrix4::Perspective(1, farPlaneDist, 1, 90);

	std::vector<Matrix4> shadowTransforms;
	Vector3 lightPos = light.GetPosition();
	shadowTransforms.emplace_back(lightProjMatrix * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(1, 0, 0), Vector3(0.0, -1.0, 0.0)));
	shadowTransforms.emplace_back(lightProjMatrix * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(-1, 0, 0), Vector3(0.0, -1.0, 0.0)));
	shadowTransforms.emplace_back(lightProjMatrix * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(0, 1, 0), Vector3(0.0, 0.0, 1.0)));
	shadowTransforms.emplace_back(lightProjMatrix * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(0, -1, 0), Vector3(0.0, 0.0, -1.0)));
	shadowTransforms.emplace_back(lightProjMatrix * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(0, 0, 1), Vector3(0.0, -1.0, 0.0)));
	shadowTransforms.emplace_back(lightProjMatrix * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(0, 0, -1), Vector3(0.0, -1.0, 0.0)));
	glUniformMatrix4fv(glGetUniformLocation(shadowShader->GetProgram(), "shadowMatrices"), 6, 0, (float*)shadowTransforms.data());
	glUniform3fv(glGetUniformLocation(shadowShader->GetProgram(), "lightPos"), 1, (float*)&lightPos);
	glUniform1f(glGetUniformLocation(shadowShader->GetProgram(), "lightFarPlane"), farPlaneDist);

	bool staticLight = light.IsStatic();
	for (auto& n : nodeList) {
		if (staticLight && !n->IsStatic())
			continue; 

		modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(shadowShader->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);
		n->Draw(shadowShader);
	}
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(),
		transparentNodeList.rend(),
		SceneNode::CompareByCameraDistance);

	std::sort(nodeList.begin(),
		nodeList.end(),
		SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNode(SceneNode* n) {

	Shader* activeShader = n->GetShader();

	//If the object has no shader assigned then don't draw it.
	if (!activeShader) {
		return;
	}

	if (currentShader != activeShader) {
		BindShader(activeShader);
		
		glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "normalTex"), 1);
		glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "reflectCube"), 2);
	}

	if (n->GetMesh()) {
		modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		textureMatrix = n->GetTextureMatrix();
		UpdateShaderMatrices();

		if (n->GetTexture()) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, n->GetTexture());
		}

		GLuint normal = n->GetNormal();

		if (normal) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normal);
		}

		if (n->IsReflective()) {
			glUniform3fv(glGetUniformLocation(activeShader->GetProgram(), "cameraPos"), 1, (float*)&scene->camera->GetPosition());
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_CUBE_MAP, scene->skybox);
		}

		n->Draw(activeShader);
	}
}

void Renderer::RenderScene() {

	BuildNodeLists(scene->root, true);
	SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawShadowMaps(false);
	DrawOpaques();
	DrawTransparents();
	DrawSkybox();
	DrawLights();
	PostProcessing();

	ClearNodeLists();
}

void Renderer::DrawOpaques() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS, 1, ~0);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	viewMatrix = scene->camera->BuildViewMatrix();

	for (auto i : nodeList)
	{
		DrawNode(i);
	}
	textureMatrix.ToIdentity();

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawTransparents() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferTransparentColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferTransparentNormalTex, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_STENCIL_TEST);
	glDepthMask(GL_FALSE);

	for (auto i : transparentNodeList)
	{
		DrawNode(i);
	}
	textureMatrix.ToIdentity();

	glEnable(GL_STENCIL_TEST);
	glDepthMask(GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::SetupLightShader(Shader* shader) {
	BindShader(shader);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthStencilTex);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "normalTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "shadowMap"), 2);

	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)&scene->camera->GetPosition());

	glUniform2f(glGetUniformLocation(shader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);
	UpdateShaderMatrices();
}

void Renderer::DrawLights() {
	glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO);

	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	//Only draw lights where something is already drawn.
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 1, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	SetupLightShader(pointLightShader);
	for (int i = 0; i < scene->pointLights.size(); ++i) {
		Light& l = scene->pointLights[i];
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, l.GetShadowMap());

 		l.SetShaderLightData(pointLightShader);
		sphere->Draw();
	}

	SetupLightShader(spotLightShader);
	for (int i = 0; i < scene->spotLights.size(); ++i) {
		Light& l = scene->spotLights[i];
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, l.GetShadowMap());

		l.SetShaderLightData(spotLightShader);
		sphere->Draw();
	}

	glDisable(GL_STENCIL_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);

	glDepthMask(GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawSkybox() {

	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(skyboxShader->GetProgram(), "cubeTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, scene->skybox);

	//DO NOT draw skybox where something is already drawn.
	glStencilFunc(GL_NOTEQUAL, 1, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glDepthMask(GL_FALSE);

	quad->Draw();
	glDepthMask(GL_TRUE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::PostProcessing() {
	outputPostProcessTex = 0;
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();

	//Combine the buffers and put the output into the postprocessing shader
	CombineBuffers();

	if (doBlur) Blur();

	PresentScene();
	projMatrix = scene->GetCameraPerspective(width, height);
}

void Renderer::CombineBuffers() {
	
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessColourTex[outputPostProcessTex], 0);

	glClear(GL_COLOR_BUFFER_BIT);
	
	BindShader(combineShader);

	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "transparentDiffuseTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bufferTransparentColourTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "skyboxTex"), 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, skyboxColourTex);

	quad->Draw();

	outputPostProcessTex = !outputPostProcessTex;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::Blur() {
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
		
	BindShader(blurShader);
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	bool isVertical = 0;
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(blurShader->GetProgram(), "sceneTex"), 0);
	for (int i = 0; i < BLUR_PASSES * 2; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessColourTex[outputPostProcessTex], 0);
		glUniform1i(glGetUniformLocation(blurShader->GetProgram(), "isVertical"), isVertical);
		glBindTexture(GL_TEXTURE_2D, postProcessColourTex[!outputPostProcessTex]);
		quad->Draw();

		isVertical = !isVertical;
		outputPostProcessTex = !outputPostProcessTex;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	BindShader(sceneShader);

	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postProcessColourTex[!outputPostProcessTex]);


	quad->Draw();
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}