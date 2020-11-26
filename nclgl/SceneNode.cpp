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
	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
	texture = 0;
	normal = 0;
	parent = NULL;
	modelScale = Vector3(1, 1, 1);
	frameTime = 0.0f;
	currentFrame = 0;

	if (mesh) {
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
		animRelativeJoints = anim->GenerateRelativeJoints(mesh->GetInverseBindPose());
	}
}

SceneNode::~SceneNode(void) {
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
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

void SceneNode::Draw(const OGLRenderer& r) {
	if (mesh) { 
		if (anim) {
			vector<Matrix4> frameMatrices;
			const Matrix4* invBindPose = mesh->GetInverseBindPose();
			const Matrix4* prevFrameData = GetRelativeJointData(currentFrame == 0 ? anim->GetFrameCount() - 1 : currentFrame - 1);
			const Matrix4* currFrameData = GetRelativeJointData(currentFrame);

			float progress = frameTime / (1.0f / anim->GetFrameRate());
			for (int j = 0; j < mesh->GetJointCount(); j++)
			{
				frameMatrices.push_back(Matrix4::Lerp(progress, currFrameData[j], prevFrameData[j]));
			}

			int j = glGetUniformLocation(shader->GetProgram(), "joints");
			glUniformMatrix4fv(j, anim->GetJointCount(), false, (float*)frameMatrices.data());
		}

		if (material) {
			for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, matTextures[i]);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, matNormals[i]);
				mesh->DrawSubMesh(i);
			}
		}
		else {
			mesh->Draw();
		}
	}
}

void SceneNode::Update(float dt) {
	if (parent) {
		worldTransform = parent->worldTransform * transform;
	}
	else {
		worldTransform = transform;
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
}

const Matrix4* SceneNode::GetRelativeJointData(unsigned int frame) const {
	if (frame >= anim->GetFrameCount()) {
		return nullptr;
	}
	int matStart = frame * anim->GetJointCount();

	Matrix4* dataStart = (Matrix4*)animRelativeJoints.data();

	return dataStart + matStart;
}