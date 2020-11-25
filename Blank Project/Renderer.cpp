#include "Renderer.h"
#include "Scene.h"

#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"

#include <algorithm>

const int POINT_LIGHT_NUM = 24;
const int SPOT_LIGHT_NUM = 40;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

	scene = new Scene();
	quad = Mesh::GenerateQuad();
	cube = Mesh::LoadFromMeshFile("OffsetCubeY.msh");
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");


	shader = new Shader("BumpVertex.glsl", "BufferFragment.glsl");
	BindShader(shader);
	texture = SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	heightMapTexture = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	heightMapBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	SetTextureRepeating(heightMapTexture, true);
	SetTextureRepeating(heightMapBump, true);

	if (!shader->LoadSuccess() || !texture || ! heightMapTexture || !heightMapBump)
		return;

	pointLights = new Light[POINT_LIGHT_NUM];
	spotLights = new SpotLight[SPOT_LIGHT_NUM];

	mesh_heightMap = new HeightMap(TEXTUREDIR"noise.png");

	Vector3 heightmapSize = mesh_heightMap->GetHeightMapSize();
	for (int i = 0; i < POINT_LIGHT_NUM; i++)
	{
		Light& l = pointLights[i];

		l.SetPosition(Vector3((rand() % (int)heightmapSize.x) - heightmapSize.x / 2,
			250.0f,
			(rand() % (int)heightmapSize.z) - heightmapSize.x / 2));

		l.SetRadius(500 + (rand() % 250));

		l.SetDiffuseColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			1));
	}

	for (int i = 0; i < SPOT_LIGHT_NUM; ++i) {
		SpotLight& l = spotLights[i];

		//l.SetPosition(Vector3(heightmapSize.x * 0.5, 600, heightmapSize.z * 0.5));

		l.SetPosition(Vector3((rand() % (int)heightmapSize.x) - heightmapSize.x / 2,
			250.0f,
			(rand() % (int)heightmapSize.z) - heightmapSize.x / 2));

		l.SetDiffuseColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			1));

		l.SetRotation(Vector3(rand() % 360, 0, 0));

		l.SetRadius(500 + (rand() % 250));
		l.SetAngle(20 + rand() % 45);
		l.SetDirection(Vector3(0, -1, 0));
	}


	for (int i = 0; i < 5; ++i) {
		SceneNode* s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
		s->SetTransform(Matrix4::Translation(Vector3(0, 100.0f, -300.0f + 100.0f + 100 * i)));
		s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
		s->SetBoundingRadius(100.0f);
		s->SetMesh(quad);
		s->SetTexture(texture);
		scene->root->AddChild(s);
	}

	mesh_roleT = Mesh::LoadFromMeshFile("Role_T.msh");
	anim_roleT = new MeshAnimation("Role_T.anm");
	anim_roleT->GenerateRelativeJoints(mesh_roleT->GetInverseBindPose());
	mat_roleT = new MeshMaterial("Role_T.mat");
	shader_roleT = new Shader("SkinningBumpVertex.glsl", "BufferFragment.glsl");

	if (!shader_roleT->LoadSuccess())
		return;

	SceneNode* role_t = new SceneNode(mesh_roleT, anim_roleT, mat_roleT, Vector4(1, 1, 1, 1), shader_roleT);
	role_t->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
	role_t->SetBoundingRadius(200.0f);
	scene->root->AddChild(role_t);

	
	SceneNode* heightMapNode = new SceneNode(mesh_heightMap);
	heightMapNode->SetBoundingRadius((mesh_heightMap->GetHeightMapSize()).Length());
	heightMapNode->SetTransform(Matrix4::Translation(Vector3(-mesh_heightMap->GetHeightMapSize().x / 2, -mesh_heightMap->GetHeightMapSize().y, -mesh_heightMap->GetHeightMapSize().z / 2)));
	heightMapNode->SetTexture(heightMapTexture);
	heightMapNode->SetNormal(heightMapBump);
	heightMapShader = new Shader("BumpVertex.glsl", "BufferFragment.glsl");
	heightMapNode->SetShader(heightMapShader);

	if (!heightMapShader->LoadSuccess())
		return;

	scene->root->AddChild(heightMapNode);

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
	delete shader;

	delete mesh_roleT;
	delete anim_roleT;
	delete mat_roleT;
	delete shader_roleT;

	delete mesh_heightMap;

	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &heightMapTexture);
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

	scene->camera->Rotate(-Window::GetMouse()->GetRelativePosition().y, -Window::GetMouse()->GetRelativePosition().x, 0);

	Matrix4 rotation = Matrix4::Rotation(scene->camera->GetYaw(), Vector3(0, 1, 0)) * Matrix4::Rotation(scene->camera->GetRoll(), Vector3(0, 0, 1));

	Vector3 forward = rotation * Vector3(0, 0, -1) * dt;
	Vector3 right = rotation * Vector3(1, 0, 0) * dt;
	Vector3 up = Vector3(0, 1, 0);
	Vector3 velocity = Vector3(90, 90, 90);

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		scene->camera->Translate(forward * velocity);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		scene->camera->Translate(-forward * velocity);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		scene->camera->Translate(-right * velocity);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		scene->camera->Translate(right * velocity);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		scene->camera->Translate(up * velocity * dt);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		scene->camera->Translate(-up * velocity.y * dt);
	}

	for (int i = 0; i < SPOT_LIGHT_NUM; i++)
	{
		spotLights[i].SetRotation(spotLights[i].GetRotation() + Vector3(1, 0, 0) * velocity * dt);
	}

	scene->camera->UpdateCamera(dt);
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

	Shader* activeShader = shader;
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

		texture = n->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

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
	for (int i = 0; i < POINT_LIGHT_NUM; ++i) {
		Light& l = pointLights[i];
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
	for (int i = 0; i < SPOT_LIGHT_NUM; ++i) {
		SpotLight& l = spotLights[i];
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