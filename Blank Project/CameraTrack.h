#pragma once
#include "../nclgl/Vector3.h"
#include "../nclgl/Camera.h"
#include <vector>

class CameraTrack{

private: 
	struct Waypoint {
		Vector3 pos;
		Vector3 rot;
		float travelTime;
	};

	Camera* target;
	std::vector<Waypoint> waypoints;
	float speedMultiplier;
	float hangTime;
	float progress;
	bool active;
	bool loop;
	int current;

public:
	CameraTrack(Camera* _cam);
	CameraTrack(Camera* _cam, float _speedMultiplier, bool loop);

	void Start();
	void Stop();
	bool IsActive() const { return active; }
	void Update(float dt);
	void AddWaypoint(Vector3 pos, Vector3 rot, float travelTime = 1);
};
