#pragma once
#include <nclgl\Vector3.h>

class SceneNode;

class Patrol {

	struct Waypoint {
		Vector3 pos;
		float pitch;
		float yaw;
		float roll;
		float travelTime;
	};

public:
	Patrol(SceneNode* target, float moveSpeed);
	~Patrol() {};


protected:
	SceneNode* target;
};