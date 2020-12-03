#pragma once
#include "../nclgl/Vector3.h"
#include "../nclgl/Camera.h"
#include <vector>
#include <algorithm>

template<typename T>
class Track{

private: 
	struct Waypoint {
		Vector3 pos;
		Vector3 rot;
		float travelTime;
	};

	T* target;
	std::vector<Waypoint> waypoints;
	float speedMultiplier;
	float progress;
	bool active;
	bool loop;
	bool smooth;
	int current;

	float Interp(float start, float end, float progress) { return smooth ? CosineInterp(start, end, progress) : Lerp(start, end, progress); }

	float Lerp(float start, float end, float progress) { return (start * (1.0f - progress) + end * progress); }

	float CosineInterp(float start, float end, float progress) {
		float cosProgress;
		cosProgress = (1 - cos(progress * PI)) / 2;
		return(start * (1 - cosProgress) + end * cosProgress);
	}

public:
	Track(T* target) {
		this->target = target;
		speedMultiplier = 1.0f;
		hangTime = 0.0f;
		progress = 0.0f;
		active = false;
		loop = false;
		current = 0;
	}

	Track(T* target, float speedMultiplier, bool loop, bool smooth) {
		this->target = target;
		this->speedMultiplier = speedMultiplier;
		this->smooth = smooth;
		this->loop = loop;
		progress = 0.0f;
		active = false;
		current = 0;
		
	}


	void Start() {
		active = true;
		current = 0;
		progress = 0;
	}

	void Stop() {
		active = false;
	}

	bool IsActive() const { return active; }

	void Update(float dt) {
		if (!active || current + 1 >= waypoints.size())
			return;

		Waypoint curr = waypoints[current];
		Waypoint next = waypoints[current + 1];

		progress += speedMultiplier * dt / next.travelTime;
		progress = std::min(progress, 1.0f);

		target->SetPosition(Vector3(Interp(curr.pos.x, next.pos.x, progress),
									Interp(curr.pos.y, next.pos.y, progress),
									Interp(curr.pos.z, next.pos.z, progress)));

		target->SetRotation(Vector3(Interp(curr.rot.x, next.rot.x, progress),
									Interp(curr.rot.y, next.rot.y, progress),
									Interp(curr.rot.z, next.rot.z, progress)));

		if (progress == 1.0f)
		{
			progress = 0;
			++current;

			if (current == waypoints.size() - 1)
			{
				active = loop;
				if (active)
					Start();
			}
		}
	}

	void AddWaypoint(Vector3 pos, Vector3 rot, float travelTime = 1) {
		waypoints.push_back({ pos,rot,travelTime });
	}
};
