#pragma once

#include "Vector4.h"
#include "Vector3.h"

class Light {
public:
	Light() { position = {}; radius = {}; diffuseColour = {}; specularColour = {}; }
	Light(const Vector3& position, const Vector4& diffuseColour, const Vector4& specularColour, float radius) {
		this->position = position;
		this->diffuseColour = diffuseColour;
		this->specularColour = specularColour;
		this->radius = radius;
	}

	~Light(void) {};

	Vector3 GetPosition() const { return position; }
	void SetPosition(const Vector3& val) { position = val; }

	void Translate(Vector3 val) { position += val; }

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }
	
	Vector4 GetDiffuseColour() const { return diffuseColour; }
	void SetDiffuseColour(const Vector4& val) { diffuseColour = val; }

	Vector4 GetSpecularColour() const { return specularColour; }
	void SetSpecularColour(const Vector4& val) { specularColour = val; }

protected:
	Vector3 position;
	float radius;
	Vector4 diffuseColour;
	Vector4 specularColour;

};

class DirectionLight {
public: 
	DirectionLight() {};
	DirectionLight(const Vector3& direction, const Vector4& diffuseColour, const Vector4& specularColour) {
		this->direction = direction;
		this->diffuseColour = diffuseColour;
		this->specularColour = specularColour;
	}

	~DirectionLight(void) {};

	Vector3 GetDirection() const { return direction; }
	void SetDirection(Vector3 val) { direction = val; }

	Vector4 GetDiffuseColour() const { return diffuseColour; }
	void SetDiffuseColour(const Vector4& val) { diffuseColour = val; }

	Vector4 GetSpecularColour() const { return specularColour; }
	void SetSpecularColour(const Vector4& val) { specularColour = val; }

protected:
	Vector3 direction;
	Vector4 specularColour;
	Vector4 diffuseColour;
};

class SpotLight : public Light {
public:
	SpotLight() {};
	SpotLight(const Vector3& position, const Vector3& direction, const Vector4& diffuseColour, const Vector4& specularColour, float radius, float coneAngle) :Light(position,diffuseColour,specularColour,radius) {
		this->coneAngle = coneAngle;
		this->direction = direction;
		this->rotation = {};
	}

	~SpotLight(void) {};

	float GetAngle() const { return coneAngle; }
	void SetAngle(float val) { coneAngle = val; }

	Vector3 GetRotation() const { return rotation; }
	void SetRotation(const Vector3& val) { rotation = val; }

	Vector3 GetDirection() const { return direction; }
	void SetDirection(Vector3 val) { direction = val; }

protected:
	float coneAngle;
	Vector3 direction;
	Vector3 rotation;
};