#include "CameraTrack.h"

CameraTrack::CameraTrack(Camera* _cam)
{
	cam = _cam;
	speedMultiplier = 1.0f;
	hangTime = 0.0f;
	progress = 0.0f;
	active = false;
	current = 0;
}

CameraTrack::CameraTrack(Camera* _cam, float _speedMultiplier)
{
	cam = _cam;
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

	progress += speedMultiplier * dt;
	progress = std::min(progress, 1.0f);

	cam->SetPosition(Vector3(cosineInterp(curr.pos.x, next.pos.x, progress),
		cosineInterp(curr.pos.y, next.pos.y, progress),
		cosineInterp(curr.pos.z, next.pos.z, progress)));

	cam->SetPitch(cosineInterp(curr.pitch, next.pitch, progress));
	cam->SetYaw(cosineInterp(curr.yaw, next.yaw, progress));
	cam->SetRoll(cosineInterp(curr.roll, next.roll, progress));

	if (progress == 1.0f)
	{
		progress = 0;
		++current;

		if (current == waypoints.size() - 1)
		{
			active = false;
		}
	}

}


void CameraTrack::AddWaypoint(Vector3 _pos, float _pitch, float _yaw, float _roll)
{
	waypoints.push_back({ _pos,_pitch,_yaw,_roll });
}