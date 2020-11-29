#pragma once
#include "../nclgl/Vector3.h"
#include "../nclgl/Camera.h"
#include <vector>

class CameraTrack{

private: 
	struct Waypoint {
		Vector3 pos;
		float pitch;
		float yaw;
		float roll;
	};

	Camera* cam;
	std::vector<Waypoint> waypoints;
	float speedMultiplier;
	float hangTime;
	float progress;
	bool active;
	int current;

public:
	CameraTrack(Camera* _cam);
	CameraTrack(Camera* _cam, float _speedMultiplier);

	void Start();
	void Update(float dt);
	void AddWaypoint(Vector3 _pos, float _pitch, float _yaw, float _roll);
};
