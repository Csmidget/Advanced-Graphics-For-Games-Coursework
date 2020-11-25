#include "Camera.h"
#include "Window.h"

void Camera::UpdateCamera(float dt) {


	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
	//	roll -= 30 * dt;
	//}
	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT)) {
	//	roll += 30 * dt;
	//}
	
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	//roll = std::min(roll, 90.0f);
	//roll = std::max(roll, -90.0f);

	if (yaw <= 0) {
		yaw += 360.0f;
	}
	if (yaw >= 360.0f) {
		yaw -= 360.0f;
	}

	if (roll <= 0) {
		roll += 360.0f;
	}
	if (roll >= 360.0f) {
		roll -= 360.0f;
	}

}

Matrix4 Camera::BuildViewMatrix() {
	return	Matrix4::Rotation(-pitch,	Vector3(1,0,0)) *
			Matrix4::Rotation(-yaw,		Vector3(0, 1, 0)) *
			Matrix4::Rotation(-roll,	Vector3(0,0,1)) *
			Matrix4::Translation(-position);
}