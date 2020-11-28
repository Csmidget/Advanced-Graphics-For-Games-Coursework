#pragma once

#include "OGLRenderer.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"

#include <vector>

class MeshAnimation;
class MeshMaterial;

class SceneNode {
public:
	SceneNode(Mesh* m = NULL, MeshMaterial* = NULL, MeshAnimation* anm = NULL, Vector4 colour = Vector4(1, 1, 1, 1), Shader * s = NULL);
	~SceneNode(void);

	void SetTransform(const Matrix4& matrix) { transform = matrix; }
	const Matrix4& GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }

	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m);

	Shader* GetShader() const { return shader; }
	void SetShader(Shader* s) { shader = s; }

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	GLuint GetTexture() const { return texture; }
	void SetTexture(GLuint tex) { texture = tex; }

	GLuint GetNormal() const { return normal; }
	void SetNormal(GLuint tex) { normal = tex; }

	MeshMaterial* GetMaterial() const { return material; }
	void SetMaterial(MeshMaterial* mat) { material = mat; }

	MeshAnimation* GetAnimation() const { return anim; }
	void SetAnimation(MeshAnimation* anim);

	void AddChild(SceneNode* s);
	void RemoveChild(SceneNode* s);

	virtual void Update(float dt);
	virtual void Draw(const Shader* externalShader = nullptr);

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() { return children.begin(); }
	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() { return children.end(); }

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b) {
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}

	bool IsStatic() const { return isStatic; }
	void MakeStatic() { isStatic = true; }
	void MakeDynamic() { isStatic = false; }


protected:
	bool HasParent(SceneNode* _parent) const;
	const Matrix4* GetRelativeJointData(unsigned int frame) const;

	SceneNode* parent;

	Shader* shader;
	Mesh* mesh;
	MeshAnimation* anim;
	std::vector<Matrix4> animRelativeJoints;

	MeshMaterial* material;
	vector <GLuint> matTextures;
	vector<GLuint> matNormals;

	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	std::vector<SceneNode*> children;

	float distanceFromCamera;
	float boundingRadius;
	float frameTime;
	int currentFrame;
	bool isStatic;
	GLuint texture;
	GLuint normal;
};