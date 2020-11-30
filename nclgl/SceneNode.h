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

	const Matrix4& GetTransform() const { return transform; }
	void SetTransform(const Vector3& translate, const Vector3& rotate = { 0,0,0 }, const Vector3& scale = { 1,1,1 });
	Matrix4 GetWorldTransform() const { return worldTransform; }

	const Matrix4& GetTextureMatrix() const { return texMatrix; }
	void SetTextureMatrix(Matrix4 val) { texMatrix = val; }

	Vector3 GetPosition() const { return position; }
	void	SetPosition(Vector3 val) { position = val; }
	void	Translate(Vector3 val) { position += val; }

	Vector3 GetRotation() const { return rotation; }
	void	SetRotation(Vector3 val) { rotation = val; }
	void	rotate(Vector3 val) { rotation += val; }

	Vector3 GetScale() const { return scale; }
	void SetScale(Vector3 val) { scale = val; }
	void Scale(Vector3 val) { scale += val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }

	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m);

	Shader* GetShader() const { return shader; }
	void SetShader(Shader* s) { shader = s; }

	bool IsReflective() const { return reflective; }
	void SetReflective(bool r) { reflective = r; }

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
	virtual void Draw(const Shader* activeShader);

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() { return children.begin(); }
	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() { return children.end(); }

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b) {
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}

	bool IsStatic() const { return isStatic; }
	void MakeStatic() { isStatic = true; }
	void MakeDynamic() { isStatic = false; }


protected:

	void UpdateTransform();
	void SetTransform(const Matrix4& matrix) { transform = matrix; }

	bool HasParent(SceneNode* _parent) const;
	const Matrix4* GetRelativeJointData(unsigned int frame) const;

	SceneNode* parent;

	Vector3 position;
	Vector3 rotation;
	Vector3 scale;

	Shader* shader;
	Mesh* mesh;
	MeshAnimation* anim;
	std::vector<Matrix4> animRelativeJoints;
	std::vector<Matrix4> frameMatrices;

	MeshMaterial* material;
	vector <GLuint> matTextures;
	vector<GLuint> matNormals;

	Matrix4 worldTransform;
	Matrix4 transform;
	Matrix4 texMatrix;
	Vector3 modelScale;
	Vector4 colour;
	std::vector<SceneNode*> children;
	

	float distanceFromCamera;
	float boundingRadius;
	float frameTime;
	int currentFrame;
	bool isStatic;
	bool reflective;
	GLuint texture;
	GLuint normal;
};