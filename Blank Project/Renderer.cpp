#include "Renderer.h"
#include "DefaultScene.h"

#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"

#include <algorithm>

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

	scene = new DefaultScene();
	quad = Mesh::GenerateQuad();
	cube = Mesh::LoadFromMeshFile("OffsetCubeY.msh");
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");

	defaultTexture = SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	defaultShader = new Shader("BumpVertex.glsl", "BufferFragment.glsl");
	BindShader(defaultShader);

	SetTextureRepeating(scene->heightMapTexture, true);
	SetTextureRepeating(scene->heightMapBump, true);

	if (!defaultShader->LoadSuccess() || !defaultTexture)
		return;

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	pointLightShader = new Shader("PointLightVertex.glsl", "PointLightFragment.glsl");
	spotLightShader = new Shader("PointLightVertex.glsl", "SpotLightFragment.glsl");
	combineShader = new Shader("CombineVertex.glsl", "CombineFragment.glsl");

	if (!pointLightShader->LoadSuccess() || !spotLightShader->LoadSuccess() || !combineShader->LoadSuccess())
		return;

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &lightingFBO);

	GLenum buffers[2] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1
	};

	//Generate our scene depth texture...
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	init = true;
}

Renderer::~Renderer(void) {

	delete scene;
	delete quad;
	delete cube;
	delete defaultShader;

	glDeleteTextures(1, &defaultTexture);

}

void Renderer::Resize(int x, int y) {
	OGLRenderer::Resize(x, y);

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);

	//Generate our scene depth texture...
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::GenerateScreenTexture(GLuint& into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::UpdateScene(float dt) {

	scene->Update(dt);

	viewMatrix = scene->camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	scene->root->Update(dt);
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
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
		BuildNodeLists(*i);
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

	Shader* activeShader = defaultShader;
	if (n->GetShader()) {
		activeShader = n->GetShader();
	}

	if (GetCurrentShader()->GetProgram() != activeShader->GetProgram()) {
		BindShader(activeShader);
		UpdateShaderMatrices();

		glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "normalTex"), 1);
	}

	if (n->GetMesh()) {
		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(activeShader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(activeShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

		defaultTexture = n->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, defaultTexture);

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
	DrawLights();
	CombineBuffers();

	ClearNodeLists();
}

void Renderer::DrawOpaques() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	modelMatrix.ToIdentity();
	viewMatrix = scene->camera->BuildViewMatrix();


	for (auto i : nodeList)
	{
		DrawNode(i);
	}

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
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

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
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

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

	quad->Draw();
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}