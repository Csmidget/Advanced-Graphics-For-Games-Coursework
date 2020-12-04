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
#include "../nclgl/ShaderManager.h"

#include <algorithm>

const int BLUR_PASSES = 10;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

	doBlur = false;
	doNeonGrid = true;
	doNeonGridColourChange = true;
	doColourCorrect = true;
	saturationPoint = 2.0f;
	currentGridColour = gridColours[0];
	currentGridColourPos = 1;
	gridColourProgress = 0;
	doubleVisionOffset = 0.0f;
	identityMat.ToIdentity();

	//Default all of our screen textures to 0. This is so that GenerateSceenTexture() recognises
	//they are empty and generates a new texture for them.
	bufferDepthStencilTex = 0;
	bufferColourTex = 0;
	bufferNormalTex = 0;
	lightDiffuseTex = 0;
	lightSpecularTex = 0;
	skyboxColourTex = 0;
	bufferTransparentColourTex = 0;
	bufferTransparentNormalTex = 0;
	nextPostProcessOutput = 0;
	postProcessColourTex[0] = 0;
	postProcessColourTex[1] = 0;
	neonGridColourTex = 0;

	scene = new DefaultScene();
	if (!scene->initialized) {
		return;
	}
	//A single update to initialize world transforms etc.
	scene->Update(0.0f);

	//Our fundamental meshes
	quad = Mesh::GenerateQuad();					//For screenspace shaders
	sphere = MeshManager::LoadMesh("Sphere.msh");	//For Lights

	projMatrix = scene->GetCameraPerspective(width, height);

	skyboxShader			= ShaderManager::LoadShader("SkyboxVertex.glsl", "SkyboxFragment.glsl");
	blurShader				= ShaderManager::LoadShader("TexturedVertex.glsl", "BlurFragment.glsl");
	pointLightShader		= ShaderManager::LoadShader("LightVertex.glsl", "PointLightShadowFragment.glsl");
	spotLightShader			= ShaderManager::LoadShader("LightVertex.glsl", "SpotLightShadowFragment.glsl");
	shadowShader			= ShaderManager::LoadShader("ShadowVertex.glsl", "ShadowFragment.glsl", "ShadowGeometry.glsl");
	combineShader			= ShaderManager::LoadShader("CombineVertex.glsl", "CombineFragment.glsl");
	neonGridShader			= ShaderManager::LoadShader("FlexibleTextureVertex.glsl", "ColouredLinesFragment.glsl");
	basicSceneShader		= ShaderManager::LoadShader("TexturedVertex.glsl", "TexturedFragment.glsl");
	colourCorrectionShader	= ShaderManager::LoadShader("TexturedVertex.glsl", "ColourCorrectFragment.glsl");
	doubleVisionShader		= ShaderManager::LoadShader("TexturedVertex.glsl", "DoubleVisionFragment.glsl");

	//Generate our projection view matrix uniform buffer
	glGenBuffers(1, &projViewBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, projViewBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, projViewBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &lightingFBO);
	glGenFramebuffers(1, &skyboxFBO);
	glGenFramebuffers(1, &postProcessFBO);
	glGenFramebuffers(1, &shadowFBO);
	glGenFramebuffers(1, &neonGridFBO);

	GLenum buffers[2] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1
	};

	//Generate our screen textures
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
	GenerateScreenTexture(neonGridColourTex);

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
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessColourTex[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, neonGridFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, neonGridColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);
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

	//This bakes our static shadow maps.
	BuildNodeLists(scene->root, false);
	DrawShadowMaps(true);
	ClearNodeLists();

	init = true;
}

Renderer::~Renderer(void) {
	delete scene;
	delete quad;

	glDeleteTextures(1, &bufferDepthStencilTex);
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferTransparentColourTex);
	glDeleteTextures(1, &bufferTransparentNormalTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteTextures(1, &skyboxColourTex);
	glDeleteTextures(2, postProcessColourTex);
	glDeleteTextures (1, &neonGridColourTex);

	glDeleteBuffers(1, &projViewBuffer);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &lightingFBO);
	glDeleteFramebuffers(1, &skyboxFBO);
	glDeleteFramebuffers(1, &postProcessFBO);
	glDeleteFramebuffers(1, &neonGridFBO);
	glDeleteFramebuffers(1, &shadowFBO);
}

void Renderer::GenerateScreenTexture(GLuint& into, bool depth) {
	
	//Generate a new gl texture if a valid GLuint isn't already provided.
	if (!into) {
		glGenTextures(1, &into);
	}

	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint internalformat = depth ? GL_DEPTH24_STENCIL8 : GL_RGBA16F;
	GLuint format = depth ? GL_DEPTH_STENCIL : GL_RGBA;
	GLuint type = depth ? GL_UNSIGNED_INT_24_8 : GL_FLOAT;

	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::Resize(int x, int y) {
	OGLRenderer::Resize(x, y);

	//Regenerate our screen textures to fit the new resolution.
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
	GenerateScreenTexture(neonGridColourTex);

	//Set out FBO's to use the regenerated textures.
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, skyboxColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, neonGridFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, neonGridColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthStencilTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::UpdateScene(float dt) {

	//Enable/Disable various effects
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
		doBlur = !doBlur;
		std::cout << "Blur " << (doBlur ? "ENABLED\n" : "DISABLED\n");
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
		doNeonGrid = !doNeonGrid;
		std::cout << "Neon Grid" << (doNeonGrid ? "ENABLED\n" : "DISABLED\n");
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {
		doColourCorrect = !doColourCorrect;
		std::cout << "Colour Correction " << (doColourCorrect ? "ENABLED\n" : "DISABLED\n");
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_4)) {
		doNeonGridColourChange = !doNeonGridColourChange;
		std::cout << "Neon Grid Colour Change " << (doNeonGridColourChange ? "ENABLED\n" : "DISABLED\n");
	}

	//Control the point at which colour becomes saturated.
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP)) {
		saturationPoint = std::min(2.0f, saturationPoint + dt);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN)) {
		saturationPoint = std::max(0.0f, saturationPoint - dt);
	}

	//Control the double vision effect
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
		doubleVisionOffset = std::min(0.2f, doubleVisionOffset +  0.1f * dt);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT)) {
		doubleVisionOffset = std::max(0.0f, doubleVisionOffset - 0.1f * dt);
	}

	if (doNeonGridColourChange) {
		gridColourProgress += 0.1f * dt;
		int nextGridColourPos = currentGridColourPos + 1;
		if (nextGridColourPos > 5) nextGridColourPos = 0;
		currentGridColour = Vector4::Lerp(gridColours[currentGridColourPos], gridColours[nextGridColourPos], gridColourProgress);
		if (gridColourProgress > 1.0f) {
			gridColourProgress = 0;
			currentGridColourPos = nextGridColourPos;
		}
	}

	scene->Update(dt);

	//Rebuild our view based on the cameras position and update the frustum for culling
	viewMatrix = scene->camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
}

void Renderer::RenderScene() {

	BuildNodeLists(scene->root, true);
	SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	//Generate dynamic shadow maps for this frame. 
	//Only objects within view frustum will be used
	DrawShadowMaps(false);
	SetProjViewBuffer(scene->GetCameraPerspective(width, height), viewMatrix);
	DrawOpaques();
	DrawTransparents();

	//Deferred rendering, light our scene
	DrawLights();
	
	//If neon grid is enabled, draw it in unlit areas
	DrawNeonGrid();

	DrawSkybox();

	SetProjViewBuffer(identityMat, identityMat);
	PostProcessing();

	ClearNodeLists();
}

void Renderer::UpdatePerObjectShaderMatrices() {
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, textureMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1, false, shadowMatrix.values);
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

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(),
		transparentNodeList.rend(),
		SceneNode::CompareByCameraDistance);

	std::sort(nodeList.begin(),
		nodeList.end(),
		SceneNode::CompareByCameraDistance);
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::DrawShadowMaps(bool staticLights) {

	int resolution = staticLights ? STATIC_SHADOW_RESOLUTION : DYNAMIC_SHADOW_RESOLUTION;
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glViewport(0, 0, resolution, resolution);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	BindShader(shadowShader);

	for (auto& l : scene->pointLights) {
		//If the light is active, and is of the correct type (static/dynamic)
		if (l->GetActive() && ((staticLights && l->IsStatic()) || !l->IsStatic()))
			DrawShadowMap(l, l->GetRadius());
	}

	for (auto& l : scene->spotLights) {
		if (l->GetActive() && ((staticLights && l->IsStatic()) || !l->IsStatic()))
			DrawShadowMap(l, l->GetRadius());
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);

}

void Renderer::DrawShadowMap(Light* light,float farPlaneDist) {

	if (light->GetShadowMap() == 0)
		light->GenerateShadowMapTexture();

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, light->GetShadowMap(), 0);

	glClear(GL_DEPTH_BUFFER_BIT);

	Matrix4 lightProjMatrix = Matrix4::Perspective(1, farPlaneDist, 1, 90);

	std::vector<Matrix4> shadowTransforms;
	Vector3 lightPos = light->GetPosition();
	shadowTransforms.emplace_back(lightProjMatrix * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(1, 0, 0), Vector3(0.0, -1.0, 0.0)));
	shadowTransforms.emplace_back(lightProjMatrix * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(-1, 0, 0), Vector3(0.0, -1.0, 0.0)));
	shadowTransforms.emplace_back(lightProjMatrix * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(0, 1, 0), Vector3(0.0, 0.0, 1.0)));
	shadowTransforms.emplace_back(lightProjMatrix * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(0, -1, 0), Vector3(0.0, 0.0, -1.0)));
	shadowTransforms.emplace_back(lightProjMatrix * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(0, 0, 1), Vector3(0.0, -1.0, 0.0)));
	shadowTransforms.emplace_back(lightProjMatrix * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(0, 0, -1), Vector3(0.0, -1.0, 0.0)));
	glUniformMatrix4fv(glGetUniformLocation(shadowShader->GetProgram(), "shadowMatrices"), 6, 0, (float*)shadowTransforms.data());
	glUniform3fv(glGetUniformLocation(shadowShader->GetProgram(), "lightPos"), 1, (float*)&lightPos);
	glUniform1f(glGetUniformLocation(shadowShader->GetProgram(), "lightFarPlane"), farPlaneDist);

	bool staticLight = light->IsStatic();
	for (auto n : nodeList) {
		//If the light is static, do not draw shadows for dynamic objects
		if (staticLight && !n->IsStatic())
			continue; 

		modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(shadowShader->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);
		n->Draw(shadowShader);
	}
}

void Renderer::SetProjViewBuffer(const Matrix4& projMatrix, const Matrix4& viewMatrix) {
	glBindBuffer(GL_UNIFORM_BUFFER, projViewBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &projMatrix);
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &viewMatrix);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Renderer::DrawOpaques() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS, 1, ~0);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	for (auto i : nodeList) {
		DrawNode(i);
	}
	textureMatrix.ToIdentity();

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawNode(SceneNode* n, Shader* shaderOverride) {

	Shader* activeShader = shaderOverride ? shaderOverride : n->GetShader();

	//If the object has no shader assigned then don't draw it.
	if (!activeShader) {
		return;
	}

	//If we're using a different shader to previous, setup some default uniforms.
	if (currentShader != activeShader) {
		BindShader(activeShader);
		
		glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "normalTex"), 1);
		glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "reflectCube"), 2);
	}

	if (n->GetMesh()) {
		modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		textureMatrix = n->GetTextureMatrix();
		UpdatePerObjectShaderMatrices();

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
			//We only need the camera position and skybox if the object is reflective (water)
			glUniform3fv(glGetUniformLocation(activeShader->GetProgram(), "cameraPos"), 1, (float*)&scene->camera->GetPosition());
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_CUBE_MAP, scene->skybox);
		}

		n->Draw(activeShader);
	}
}

void Renderer::DrawTransparents() {
	//Reuse the same FBO as opaques, just switch out the target textures.
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferTransparentColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferTransparentNormalTex, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	//We don't want transparents to write to the depth or stencil buffers
	glDisable(GL_STENCIL_TEST);
	glDepthMask(GL_FALSE);

	for (auto i : transparentNodeList) {
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
	//Set the second bit if we do draw the light. This lets us test for
	//areas that have been lit.
	glStencilFunc(GL_EQUAL, 3, 1);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	SetupLightShader(pointLightShader);
	for (auto l : scene->pointLights) {
		if (!l->GetActive())
			continue;

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, l->GetShadowMap());

		l->SetShaderLightData(pointLightShader);
		sphere->Draw();
	}

	SetupLightShader(spotLightShader);
	for (auto l : scene->spotLights) {
		if (!l->GetActive())
			continue;

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, l->GetShadowMap());

		l->SetShaderLightData(spotLightShader);
		sphere->Draw();
	}

	glDisable(GL_STENCIL_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);

	glDepthMask(GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawNeonGrid() {
	//We always want to clear the neon Grid texture as it is used in the combinebuffers shader
	glBindFramebuffer(GL_FRAMEBUFFER, neonGridFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	if (!doNeonGrid) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return;
	}

	//We don't care about depth for the neon grid. If it passes the stencil test
	//then it should definitely be drawn.
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	//Draw only in areas that have NOT been lit at all.
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 3, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	BindShader(neonGridShader);
	glUniform4fv(glGetUniformLocation(neonGridShader->GetProgram(), "colour"), 1, (float*)&currentGridColour);
	glUniform1i(glGetUniformLocation(neonGridShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(neonGridShader->GetProgram(), "normalTex"), 1);
	glUniform1i(glGetUniformLocation(neonGridShader->GetProgram(), "jointCount"), 0);

	for (auto i : nodeList) {
		DrawNode(i, neonGridShader);
	}

	glDisable(GL_STENCIL_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawSkybox() {

	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	BindShader(skyboxShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, scene->skybox);

	//Only draw the skybox where we haven't drawn land.
	//If the neon grid is enabled we also want the skybox to appear behind it, so
	//draw in all areas that were left unlit.
	glEnable(GL_STENCIL_TEST);
	doNeonGrid ? glStencilFunc(GL_NOTEQUAL, 3, ~0) : glStencilFunc(GL_EQUAL, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	quad->Draw();

	glStencilFunc(GL_ALWAYS, 1, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glDisable(GL_STENCIL_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::PostProcessing() {
	nextPostProcessOutput = 0;
	
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	//Combine the buffers and put the output into the postprocessing shader
	CombineBuffers();

	if (doBlur) Blur();
	DoubleVision();
	PresentScene();
	projMatrix = scene->GetCameraPerspective(width, height);
}

void Renderer::CombineBuffers() {
	//This takes in all of the graphics we've generated and produces a combined image ready for post processing.

	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessColourTex[nextPostProcessOutput], 0);

	glClear(GL_COLOR_BUFFER_BIT);
	
	BindShader(combineShader);

	float ambient = doNeonGrid ? 0.0f : 0.01f;
	float transparentAmbient = doNeonGrid ? 0.0f : 0.01f;

	glUniform1f(glGetUniformLocation(combineShader->GetProgram(), "ambient"), ambient);
	glUniform1f(glGetUniformLocation(combineShader->GetProgram(), "transparentAmbient"), transparentAmbient);
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

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "neonGridTex"), 6);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, neonGridColourTex);

	quad->Draw();

	nextPostProcessOutput = !nextPostProcessOutput;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_STENCIL_TEST);
}

void Renderer::Blur() {
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
	BindShader(blurShader);
	UpdatePerObjectShaderMatrices();

	bool isVertical = 0;
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(blurShader->GetProgram(), "sceneTex"), 0);
	for (int i = 0; i < BLUR_PASSES * 2; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessColourTex[nextPostProcessOutput], 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glUniform1i(glGetUniformLocation(blurShader->GetProgram(), "isVertical"), isVertical);
		glBindTexture(GL_TEXTURE_2D, postProcessColourTex[!nextPostProcessOutput]);
		quad->Draw();

		isVertical = !isVertical;
		nextPostProcessOutput = !nextPostProcessOutput;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DoubleVision() {
	//No point if there is no offset.
	if (doubleVisionOffset == 0.0f)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
	BindShader(doubleVisionShader);
	UpdatePerObjectShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessColourTex[nextPostProcessOutput], 0);
	glUniform1f(glGetUniformLocation(doubleVisionShader->GetProgram(), "doubleXOffset"), doubleVisionOffset);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, postProcessColourTex[!nextPostProcessOutput]);

	quad->Draw();

	nextPostProcessOutput = !nextPostProcessOutput;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::PresentScene() {
	//Performs final colour correction (if enabled) and draws to the output buffer (finally)

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	Shader* shader = doColourCorrect ? colourCorrectionShader : basicSceneShader;
	BindShader(shader);

	UpdatePerObjectShaderMatrices();

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "saturationPoint"), saturationPoint);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postProcessColourTex[!nextPostProcessOutput]);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	quad->Draw();
}
