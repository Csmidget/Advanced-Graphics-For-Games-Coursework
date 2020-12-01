#pragma once
#include "../nclgl/SceneNode.h"
#include <string>


class Mesh;

//Circuit board needs some special uniforms passed in, stores extra textures, and needs a special update loop.
class CircuitBoardNode : public SceneNode {

public:

	CircuitBoardNode(Mesh* mesh);
	~CircuitBoardNode() { SceneNode::~SceneNode(); }
	virtual void Draw(const Shader* activeShader) override;
	virtual void Update(float dt) override;

protected:
	GLuint background;
	GLuint circuit;
	GLuint overlay;
	float texOffset;
};