#pragma once

#include "Matrix4.h"
#include "Vector3.h"

class Camera {
public:

	Camera(void) {
		rotation = {};
		position = {};
		nearPlane = 1.0f;
		farPlane = 10000.0f;
		fov = 45.0f;
	};

	Camera( Vector3 position, Vector3 rotation, float nearPlane, float farPlane, float fov) {
		this->rotation = rotation;
		this->position = position;
		this->nearPlane = nearPlane;
		this->farPlane = farPlane;
		this->fov = fov;
	};

	~Camera(void) {};

	void UpdateCamera(float dt = 1.0f);

	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const			{ return position; }
	void	SetPosition(Vector3 val)	{ position = val; }
	void	Translate(Vector3 val) { position += val; }

	Vector3 GetRotation() const { return rotation; }
	void	SetRotation(Vector3 val) { rotation = val; }
	void Rotate(Vector3 val) { rotation += val; }

	float GetNearPlane() const { return nearPlane; }
	void SetNearPlane(float val) { nearPlane = val; }

	float GetFarPlane() const { return farPlane; }
	void SetFarPlane(float val) { farPlane = val; }

	float GetFOV() const { return fov; }
	void SetFOV(float val) { fov = val; }


protected:
	float nearPlane;
	float farPlane;
	float fov;

	Vector3 position;
	Vector3 rotation;
};