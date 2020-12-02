#include "TrackPrefabs.h"

namespace Prefabs {
	Track<Camera>* BuildCameraTrack(Camera* cam)
	{
		auto track = new Track<Camera>(cam, 0.5f, true, true);

		track->AddWaypoint(Vector3(0, 0.0, 10.0), Vector3(-45, 0, 0));
		track->AddWaypoint(Vector3(20.9, -12.1, -25.3), Vector3(-1.73, -12.3, 0), 5.0f);
		track->AddWaypoint(Vector3(7.756, -3.112, 26.47), Vector3(-31.82, -33.3, 0), 5.0f);
		track->AddWaypoint(Vector3(60.3, 25.02, 53.88), Vector3(-31.51, 43.01, 0), 8.0f);
		track->AddWaypoint(Vector3(60.25, -14.15, 70.08), Vector3(-4.6, 64.4, 0), 6.0f);
		track->AddWaypoint(Vector3(-50.88, 20.7, 57.38), Vector3(-27.19, -43.4, 0), 10.0f);
		track->AddWaypoint(Vector3(-152, -5.9, -83.24), Vector3(-8.13, 66.58, 0), 10.0f);
		track->AddWaypoint(Vector3(-183, 2.829, -47.45), Vector3(-14.92, 191.9, 0), 6.0f);
		track->AddWaypoint(Vector3(-199.8, -12.81, 4.214), Vector3(-4.912, 270.4, 0), 6.0f);
		track->AddWaypoint(Vector3(-33.74, -9.024, 0.5379), Vector3(-7.222, 283, 0), 10.0f);
		track->AddWaypoint(Vector3(0, 0.0, 10.0), Vector3(-45, 360, 0), 4.0f);
		return track;
	}

	Track<SceneNode>* BuildCompoundPatrol(SceneNode* target) {
		auto track = new Track<SceneNode>(target, 0.5f, true, false);
		track->AddWaypoint(Vector3(-10, -15.4, -10), Vector3(0, 0, 0));
		track->AddWaypoint(Vector3(-10, -15.4, 10), Vector3(0, 0, 0), 5);
		track->AddWaypoint(Vector3(-10, -15.4, 10), Vector3(0, 90, 0));
		track->AddWaypoint(Vector3(10, -15.4, 10), Vector3(0, 90, 0), 5);
		track->AddWaypoint(Vector3(10, -15.4, 10), Vector3(0, 180, 0));
		track->AddWaypoint(Vector3(10, -15.4, -10), Vector3(0, 180, 0), 5);
		track->AddWaypoint(Vector3(10, -15.4, -10), Vector3(0, 270, 0));
		track->AddWaypoint(Vector3(-10, -15.4, -10), Vector3(0, 270, 0), 5);
		track->AddWaypoint(Vector3(-10, -15.4, -10), Vector3(0, 360, 0));
		track->Start();
		return track;
	}

	Track<SceneNode>* BuildBottomRightPatrol(SceneNode* target) {
		auto track = new Track<SceneNode>(target, 0.5f, true, false);
		track->AddWaypoint(Vector3(67.3, -15.4, 38.92), Vector3(0, 58.5, 0));
		track->AddWaypoint(Vector3(175, -15.4, 105), Vector3(0, 58.5, 0), 20);
		track->AddWaypoint(Vector3(175, -15.4, 105), Vector3(0, 238.5, 0));
		track->AddWaypoint(Vector3(67.3, -15.4, 38.92), Vector3(0, 238.5, 0),20);
		track->AddWaypoint(Vector3(67.3, -15.4, 38.92), Vector3(0, 58.5, 0));
		track->Start();
		return track;
	}
	Track<SceneNode>* BuildTopRightPatrol(SceneNode* target) {
		auto track = new Track<SceneNode>(target, 0.5f, true, false);
		track->AddWaypoint(Vector3( 68.34, -15.4, -61.13), Vector3(0, 149.1, 0));
		track->AddWaypoint(Vector3( 148.4, -15.4, -187.3), Vector3(0, 149.1, 0), 20);
		track->AddWaypoint(Vector3(148.4, -15.4, -187.3), Vector3(0, -30.9, 0));
		track->AddWaypoint(Vector3(68.34, -15.4, -61.13), Vector3(0, -30.9, 0), 20);
		track->AddWaypoint(Vector3(68.34, -15.4, -61.13), Vector3(0, 149.1, 0));
		track->Start();
		return track;
	}
	Track<SceneNode>* BuildLeftPatrol(SceneNode* target) {
		auto track = new Track<SceneNode>(target, 0.5f, true, false);
		track->AddWaypoint(Vector3(-62.03, -15.4, 3.223), Vector3(0, -90, 0));
		track->AddWaypoint(Vector3(-178.1, -15.4, 3.518), Vector3(0, -90, 0), 20);
		track->AddWaypoint(Vector3(-178.1, -15.4, 3.518), Vector3(0,  90, 0));
		track->AddWaypoint(Vector3(-62.03, -15.4, 3.223), Vector3(0,  90, 0), 20);
		track->AddWaypoint(Vector3(-62.03, -15.4, 3.223), Vector3(0, -90, 0));
		track->Start();
		return track;
	}
}