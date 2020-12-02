#pragma once
#include "../nclgl/SceneNode.h"
#include <string>

class HeightMap;

class HeightMapNode : public SceneNode {

public:

	HeightMapNode(HeightMap* mesh,std::string assignMap = "", std::string diffuse1 = "", std::string diffuse2 = "", std::string normal1 = "", std::string normal2 = "");
	~HeightMapNode() { SceneNode::~SceneNode(); }
	virtual void Draw(const Shader* activeShader) override;

protected:
	GLuint diffuse[2];
	GLuint normal[2];
	GLuint assignMap;
	Vector3 heightMapSize;

};