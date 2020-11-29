#pragma once

#include "Matrix4.h"
#include "Vector3.h"

class Camera {
public:

	Camera(void) {
		yaw = 0.0f;
		pitch = 0.0f;
		roll = 0.0f;
		nearPlane = 1.0f;
		farPlane = 10000.0f;
		fov - 45.0f;
	};

	Camera(float pitch, float yaw, float roll, Vector3 position, float nearPlane, float farPlane, float fov) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->roll = roll;
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

	float GetYaw() const { return yaw; }
	void  SetYaw(float y) { yaw = y; }

	float GetPitch() const { return pitch; }
	void  SetPitch(float p) { pitch = p; }

	float GetRoll() const { return roll; }
	void  SetRoll(float p) { roll = p; }

	float GetNearPlane() const { return nearPlane; }
	void SetNearPlane(float val) { nearPlane = val; }

	float GetFarPlane() const { return farPlane; }
	void SetFarPlane(float val) { farPlane = val; }

	float GetFOV() const { return fov; }
	void SetFOV(float val) { fov = val; }

	void Rotate(float _pitch, float _yaw, float _roll) {pitch += _pitch; yaw += _yaw; roll += _roll; }

protected:
	float yaw;
	float pitch;
	float roll;

	float nearPlane;
	float farPlane;
	float fov;

	Vector3 position;
};