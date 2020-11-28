#pragma once

#include "Vector4.h"
#include "Vector3.h"
#include <glad/glad.h>

class Shader;

#define STATIC_SHADOW_RESOLUTION 2048
#define DYNAMIC_SHADOW_RESOLUTION 2048

class Light {

public:

	Light(const Vector4& diffuseColour = {}, const Vector4& specularColour = {}, const Vector3& position = {}) {
		this->position = position;
		this->diffuseColour = diffuseColour;
		this->specularColour = specularColour;
		this->isStatic = false;
		this->shadowMap = 0;
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

};

class PointLight : public Light{
public:
	PointLight() : Light() {; radius = 0; }
	PointLight(const Vector3& position, const Vector4& diffuseColour, const Vector4& specularColour, float radius) : Light(diffuseColour,specularColour,position) {
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
	DirectionLight(const Vector3& direction, const Vector4& diffuseColour, const Vector4& specularColour) :Light(diffuseColour,specularColour) {
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
	SpotLight() : Light() { radius = 0; this->coneAngle = 0; direction = {}; rotation = {}; }
	SpotLight(const Vector3& position, const Vector3& direction, const Vector4& diffuseColour, const Vector4& specularColour, float radius, float coneAngle) :Light(diffuseColour,specularColour,position) {
		this->radius = radius;
		this->coneAngle = coneAngle;
		this->direction = direction;
		this->rotation = {};
	}

	~SpotLight(void) {};

	void SetShaderLightData(const Shader* shader) override;

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	float GetAngle() const { return coneAngle; }
	void SetAngle(float val) { coneAngle = val; }

	Vector3 GetRotation() const { return rotation; }
	void SetRotation(const Vector3& val) { rotation = val; }

	Vector3 GetDirection() const { return direction; }
	void SetDirection(Vector3 val) { direction = val; }

protected:
	float radius;
	float coneAngle;
	Vector3 direction;
	Vector3 rotation;
};