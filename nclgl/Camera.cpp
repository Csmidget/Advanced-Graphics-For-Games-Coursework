#include "Camera.h"
#include "Window.h"
#include <algorithm>

void Camera::UpdateCamera(float dt) {
	
	rotation.x = std::min(rotation.x, 90.0f);
	rotation.x = std::max(rotation.x, -90.0f);

	if (rotation.y <= 0) {
		rotation.y += 360.0f;
	}
	if (rotation.y >= 360.0f) {
		rotation.y -= 360.0f;
	}

	if (rotation.z <= 0) {
		rotation.z += 360.0f;
	}
	if (rotation.z >= 360.0f) {
		rotation.z -= 360.0f;
	}
}

Matrix4 Camera::BuildViewMatrix() {
	return	Matrix4::Rotation(-rotation.x,	Vector3(1,0,0)) *
			Matrix4::Rotation(-rotation.y,		Vector3(0, 1, 0)) *
			Matrix4::Rotation(-rotation.z,	Vector3(0,0,1)) *
			Matrix4::Translation(-position);
}