#include "SceneNode.h"
#include "MeshAnimation.h"
#include "MeshMaterial.h"
#include "TextureManager.h"

SceneNode::SceneNode(Mesh* m,  MeshMaterial* mat, MeshAnimation* anm, Vector4 colour, Shader* s) {
	this->mesh = m;
	this->anim = anm;
	this->material = mat;

	this->shader = s;
	this->colour = colour;
	position = {};
	rotation = {};
	scale = { 1,1,1 };
	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
	texture = 0;
	normal = 0;
	parent = NULL;
	modelScale = Vector3(1, 1, 1);
	frameTime = 0.0f;
	currentFrame = 0;
	reflective = false;
	isStatic = false;
	texMatrix.ToIdentity();

	if (this->mesh && this->material) {
		for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
			const MeshMaterialEntry* matEntry = material->GetMaterialForLayer(i);

			const string* filename = nullptr;
			GLuint texID = 0;
			GLuint normID = 0;
			if (matEntry->GetEntry("Diffuse", &filename)) {
				string path = TEXTUREDIR + *filename;

				texID = TextureManager::LoadTexture(path.c_str(), SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
			}

			if (matEntry->GetEntry("Bump", &filename)) {
				string path = TEXTUREDIR + *filename;

				normID = TextureManager::LoadTexture(path.c_str(), SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
			}

			matTextures.emplace_back(texID);
			matNormals.emplace_back(normID);
		}
	}

	if (anim) {
		animRelativeJoints = anim->GenerateRelativeJoints(this->mesh->GetInverseBindPose());
	}
}

SceneNode::~SceneNode(void) {
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
}

void SceneNode::SetTransform(const Vector3& translate, const Vector3& rotate, const Vector3& scale) {
	
	this->position = translate;
	this->rotation = rotate;
	this->scale = scale;
	
	UpdateTransform();
}

void SceneNode::UpdateTransform() {

	transform = Matrix4::Translation(position) *
				Matrix4::Scale(scale) *
				Matrix4::Rotation(rotation.x, { 1,0,0 }) *
				Matrix4::Rotation(rotation.y, { 0,1,0 }) *
				Matrix4::Rotation(rotation.z, { 0,0,1 });
}

void SceneNode::AddChild(SceneNode* s) {
	if (!HasParent(s)) {
		children.push_back(s);
		s->parent = this;
	}
}

void SceneNode::RemoveChild(SceneNode* s) {
	for (auto i = children.begin(); i < children.end(); i++) {
		if (*i == s) {
			(*i)->parent = nullptr;
			children.erase(i);
			break;
		}
	}
}

void SceneNode::SetAnimation(MeshAnimation* anim) {
	this->anim = anim;

	animRelativeJoints = anim->GenerateRelativeJoints(mesh->GetInverseBindPose());
}

void SceneNode::SetMesh(Mesh* mesh) {
	this->mesh = mesh;

	if (anim)
		animRelativeJoints = anim->GenerateRelativeJoints(mesh->GetInverseBindPose());
}

bool SceneNode::HasParent(SceneNode* _parent) const {
	return parent == nullptr ? false : parent == _parent || parent->HasParent(_parent);
}

void SceneNode::Draw(const Shader* activeShader) {
	if (mesh) { 
		if (anim) {
			glUniformMatrix4fv(glGetUniformLocation(activeShader->GetProgram(), "joints"), anim->GetJointCount(), false, (float*)frameMatrices.data());
			glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "hasJoints"), true);
		}
		else {
			glUniform1i(glGetUniformLocation(activeShader->GetProgram(), "hasJoints"), false);
		}

		if (mesh->GetSubMeshCount() > 0) {
			for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
				if (material) {
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, matTextures[i]);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, matNormals[i]);
				}
				mesh->DrawSubMesh(i);
			}
		}	
		else {
			if (material) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, matTextures[0]);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, matNormals[0]);
			}
			mesh->Draw();
		}
	}
}

void SceneNode::Update(float dt) {

	UpdateTransform();

	if (parent) {
		worldTransform = parent->worldTransform * transform;
		worldScale = parent->worldScale * scale;
	}
	else {
		worldTransform = transform;
		worldScale = scale;
	}

	if (anim) {
		frameTime -= dt;
		while (frameTime < 0.0f) {
			currentFrame = (currentFrame + 1) % anim->GetFrameCount();
			frameTime += 1.0f / anim->GetFrameRate();
		}
	}

	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->Update(dt);
	}

	if (anim) {
		frameMatrices.clear();
		const Matrix4* invBindPose = mesh->GetInverseBindPose();
		const Matrix4* prevFrameData = GetRelativeJointData(currentFrame == 0 ? anim->GetFrameCount() - 1 : currentFrame - 1);
		const Matrix4* currFrameData = GetRelativeJointData(currentFrame);

		float progress = frameTime /(1.0f / anim->GetFrameRate());
		for (unsigned int j = 0; j < mesh->GetJointCount(); j++) {
			frameMatrices.push_back(Matrix4::Lerp(progress, currFrameData[j], prevFrameData[j]));
		}
	}
}

const Matrix4* SceneNode::GetRelativeJointData(unsigned int frame) const {
	if (frame >= anim->GetFrameCount()) {
		return nullptr;
	}
	int matStart = frame * anim->GetJointCount();

	Matrix4* dataStart = (Matrix4*)animRelativeJoints.data();

	return dataStart + matStart;
}