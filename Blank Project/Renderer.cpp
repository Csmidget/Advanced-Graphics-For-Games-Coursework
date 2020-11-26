#include "Renderer.h"
#include "DefaultScene.h"

#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"
#include "../nclgl/TextureManager.h"

#include <algorithm>

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

	scene = new DefaultScene();

	if (!scene->initialized) {
		return;
	}

	defaultShader = new Shader("basicVertex.glsl", "colourFragment.glsl");
	BindShader(defaultShader);

	quad = Mesh::GenerateQuad();
	cube = Mesh::LoadFromMeshFile("OffsetCubeY.msh");
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");

	bufferDepthStencilTex = 0;
	bufferColourTex = 0;
	bufferNormalTex = 0;
	lightDiffuseTex = 0;
	lightSpecularTex = 0;
	skyboxColourTex = 0;

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	pointLightShader = new Shader("PointLightVertex.glsl", "PointLightFragment.glsl");
	spotLightShader = new Shader("PointLightVertex.glsl", "SpotLightFragment.glsl");
	combineShader = new Shader("CombineVertex.glsl", "CombineFragment.glsl");
	skyboxShader = new Shader("SkyboxVertex.glsl", "SkyboxFragment.glsl");

	if (!defaultShader->LoadSuccess() ||!pointLightShader->LoadSuccess() || !spotLightShader->LoadSuccess() || !combineShader->LoadSuccess() || !skyboxShader->LoadSuccess())
		return;

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &lightingFBO);
	glGenFramebuffers(1, &skyboxFBO);

	GLenum buffers[2] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1
	};

	//Generate our scene depth texture...
	GenerateScreenTexture(bufferDepthStencilTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);
	GenerateScreenTexture(skyboxColourTex);

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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	init = true;
}

Renderer::~Renderer(void) {

	delete scene;
	delete quad;
	delete cube;

	delete defaultShader;
	delete pointLightShader;
	delete spotLightShader;
	delete combineShader;
	delete skyboxShader;

	glDeleteTextures(1, &bufferDepthStencilTex);
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteTextures(1, &skyboxColourTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &lightingFBO);
	glDeleteFramebuffers(1, &skyboxFBO);

	TextureManager::Cleanup();
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
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);
	GenerateScreenTexture(skyboxColourTex);

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

	scene->Update(dt);

	viewMatrix = scene->camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	scene->root->Update(dt);
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - scene->camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		//if (from->GetColour().w < 1.0f) {
		//	transparentNodeList.push_back(from);
		//}
		//else {
			nodeList.push_back(from);
		//}
	}

	for (auto i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists(*i);
	}
}

void Renderer::SortNodeLists() {
	//std::sort(transparentNodeList.rbegin(),
	//	transparentNodeList.rend(),
	//	SceneNode::CompareByCameraDistance);

	std::sort(nodeList.begin(),
		nodeList.end(),
		SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNode(SceneNode* n) {

	Shader* activeShader = defaultShader;
	if (n->GetShader()) {
		activeShader = n->GetShader();
	}

	if (GetCurrentShader()->GetProgram() != activeShader->GetProgram()) {
		BindShader(activeShader);
		
		glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "normalTex"), 1);
	}

	if (n->GetMesh()) {
		modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

		UpdateShaderMatrices();

		//glUniformMatrix4fv(glGetUniformLocation(activeShader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(activeShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

		if (n->GetTexture()) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, n->GetTexture());
		}

		GLuint normal = n->GetNormal();

		if (normal) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normal);
		}

		n->Draw(*this);
	}
}

void Renderer::RenderScene() {
	BuildNodeLists(scene->root);
	SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

 	DrawOpaques();
	DrawSkybox();
	DrawLights();
	CombineBuffers();

	ClearNodeLists();
}

void Renderer::DrawOpaques() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS, 2, ~0);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	modelMatrix.ToIdentity();
	viewMatrix = scene->camera->BuildViewMatrix();

	for (auto i : nodeList)
	{
		DrawNode(i);
	}


	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawLights() {
	glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO);
	BindShader(pointLightShader);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniform1i(glGetUniformLocation(pointLightShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthStencilTex);

	glUniform1i(glGetUniformLocation(pointLightShader->GetProgram(), "normalTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(pointLightShader->GetProgram(), "cameraPos"), 1, (float*)&scene->camera->GetPosition());

	glUniform2f(glGetUniformLocation(pointLightShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(pointLightShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);

	UpdateShaderMatrices();
	for (int i = 0; i < scene->pointLights.size(); ++i) {
		Light& l = scene->pointLights[i];
		SetShaderLight(l);
		sphere->Draw();
	}

	BindShader(spotLightShader);

	glUniform1i(glGetUniformLocation(spotLightShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthStencilTex);

	glUniform1i(glGetUniformLocation(spotLightShader->GetProgram(), "normalTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(spotLightShader->GetProgram(), "cameraPos"), 1, (float*)&scene->camera->GetPosition());

	glUniform2f(glGetUniformLocation(spotLightShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	glUniformMatrix4fv(glGetUniformLocation(spotLightShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);

	UpdateShaderMatrices();
	for (int i = 0; i < scene->spotLights.size(); ++i) {
		SpotLight& l = scene->spotLights[i];
		SetShaderLight(l);
		sphere->Draw();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);

	glDepthMask(GL_TRUE);
	glClearColor(0.2f, 0.2f, 0.2f, 1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawSkybox() {

	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUniform1i(glGetUniformLocation(skyboxShader->GetProgram(), "cubeTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, scene->skybox);

	//DO NOT draw skybox where something is already drawn.
	glStencilFunc(GL_NOTEQUAL, 2, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();
	glDepthMask(GL_TRUE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CombineBuffers() {
	BindShader(combineShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
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

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "skyboxTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, skyboxColourTex);

	quad->Draw();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
}

void Renderer::ClearNodeLists() {
	//transparentNodeList.clear();
	nodeList.clear();
}