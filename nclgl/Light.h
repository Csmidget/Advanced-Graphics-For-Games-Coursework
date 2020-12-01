#pragma once

#include "Vector4.h"
#include "Vector3.h"
#include <glad/glad.h>

class Shader;

#define STATIC_SHADOW_RESOLUTION 1024
#define DYNAMIC_SHADOW_RESOLUTION 1024

class Light {

public:

	Light(const Vector3& position = {}, const Vector4& diffuseColour = {}, const Vector4& specularColour = {}) {
		this->position = position;
		this->diffuseColour = diffuseColour;
		this->specularColour = specularColour;
		this->isStatic = false;
		this->shadowMap = 0;
		this->active = true;
	}

	~Light() {
		glDeleteTextures(1, &shadowMap);
	}

	GLuint GenerateShadowMapTexture();

	virtual void SetShaderLightData(const Shader* shader) = 0;

	virtual Vector4 GetDiffuseColour() const { return diffuseColour; }
	virtual void SetDiffuseColour(const Vector4& val) { diffuseColour = val; }

	virtual Vector4 GetSpecularColour() const { return specularColour; }
	virtual void SetSpecularColour(const Vector4& val) { specularColour = val; }

	Vector3 GetPosition() const { return position; }
	void SetPosition(const Vector3& val) { position = val; }

	virtual void Translate(Vector3 val) { position += val; }

	void SetActive(bool val) { active = val; }
	bool GetActive() const { return active; }

	bool IsStatic() const { return isStatic; }
	void MakeStatic() { isStatic = true; }
	void MakeDynamic() { isStatic = false; }

	GLuint GetShadowMap() const { return shadowMap; }

private:
	Vector3 position;
	Vector4 specularColour;
	Vector4 diffuseColour;
	GLuint shadowMap;
	bool isStatic;
	bool active;
};

class PointLight : public Light{
public:
	PointLight() : Light() {; radius = 0; }
	PointLight(const Vector3& position, const Vector4& diffuseColour, const Vector4& specularColour, float radius) : Light(position,diffuseColour,specularColour) {
		this->radius = radius;
	}

	~PointLight(void) {};

	void SetShaderLightData(const Shader* shader) override;

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

protected:
	float radius;
};

class DirectionLight : public Light {
public: 
	DirectionLight() : Light() { direction = {}; }
	DirectionLight(const Vector3& direction, const Vector4& diffuseColour, const Vector4& specularColour) :Light({ 0,0,0 }, diffuseColour, specularColour) {
		this->direction = direction;
	}

	~DirectionLight(void) {};


	void SetShaderLightData(const Shader* shader) override;

	Vector3 GetDirection() const { return direction; }
	void SetDirection(Vector3 val) { direction = val; }

private: 
	//Directional lights do not have a position. Hide this functionality from the outside world. Not perfect, but saves on replicating 
	//positional functions in other child classes of Light.
	using Light::GetPosition;
	using Light::SetPosition;
	using Light::Translate;

protected:
	Vector3 direction;

};

class SpotLight : public Light {
public:
	SpotLight(const Vector3 position = {}, const Vector3 rotation = {}, float radius = 50.0f, float coneAngle = 30.0f, const Vector4& diffuseColour = { 1,1,1,1 }, const Vector4& specularColour = { 1,1,1,1 }) :Light(position, diffuseColour, specularColour) {
		this->radius = radius;
		this->coneAngle = coneAngle;
		this->rotation = rotation;
	}

	~SpotLight(void) {};

	void SetShaderLightData(const Shader* shader) override;

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	float GetAngle() const { return coneAngle; }
	void SetAngle(float val) { coneAngle = val; }

	Vector3 GetRotation() const { return rotation; }
	void SetRotation(const Vector3& val) { rotation = val; }
	void Rotate(const Vector3& val) { rotation += val; }

protected:
	float radius;
	float coneAngle;
	Vector3 rotation;
};