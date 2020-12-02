#include "CameraTrack.h"

CameraTrack::CameraTrack(Camera* _cam)
{
	target = _cam;
	speedMultiplier = 1.0f;
	hangTime = 0.0f;
	progress = 0.0f;
	active = false;
	current = 0;
}

CameraTrack::CameraTrack(Camera* _cam, float _speedMultiplier, bool loop)
{
	target = _cam;
	speedMultiplier = _speedMultiplier;
	hangTime = 0.0f;
	progress = 0.0f;
	active = false;
	current = 0;
}

void CameraTrack::Start()
{
	active = true;
	current = 0;
	progress = 0;
}

void CameraTrack::Stop() {
	active = false;
}

float lerp(float start, float end, float progress)
{
	return (start * (1.0f - progress) + end * progress);
}

float cosineInterp(float start, float end, float progress)
{
	double cosProgress;
	cosProgress = (1 - cos(progress * PI)) / 2;
	return(start * (1 - cosProgress) + end * cosProgress);
}

void CameraTrack::Update(float dt)
{
	if (!active || current+1 >= waypoints.size())
		return;

	Waypoint curr = waypoints[current];
	Waypoint next = waypoints[current+1];

	progress += speedMultiplier * dt / next.travelTime;
	progress = std::min(progress, 1.0f);

	target->SetPosition(Vector3(cosineInterp(curr.pos.x, next.pos.x, progress),
								cosineInterp(curr.pos.y, next.pos.y, progress),
								cosineInterp(curr.pos.z, next.pos.z, progress)));

	target->SetRotation(Vector3(cosineInterp(curr.rot.x, next.rot.x, progress),
								cosineInterp(curr.rot.y, next.rot.y, progress),
								cosineInterp(curr.rot.z, next.rot.z, progress)));

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


void CameraTrack::AddWaypoint(Vector3 pos, Vector3 rot,float travelTime)
{
	waypoints.push_back({ pos,rot,travelTime});
}