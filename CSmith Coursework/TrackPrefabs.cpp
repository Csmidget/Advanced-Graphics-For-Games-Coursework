#include "TrackPrefabs.h"

namespace Prefabs {
	Track<Camera>* BuildCameraTrack(Camera* cam)
	{
		auto track = new Track<Camera>(cam, 0.5f, true, true);

		track->AddWaypoint(Vector3(0.0f, 0.0f, 10.0f), Vector3(-45.0f, 0.0f, 0.0f));
		track->AddWaypoint(Vector3(20.9f, -12.1f, -25.3f), Vector3(-1.73f, -12.3f, 0.0f), 5.0f);
		track->AddWaypoint(Vector3(7.756f, -3.112f, 26.47f), Vector3(-31.82f, -33.3f, 0.0f), 5.0f);
		track->AddWaypoint(Vector3(60.3f, 25.02f, 53.88f), Vector3(-31.51f, 43.01f, 0.0f), 8.0f);
		track->AddWaypoint(Vector3(60.25f, -14.15f, 70.08f), Vector3(-4.6f, 64.4f, 0.0f), 6.0f);
		track->AddWaypoint(Vector3(-50.88f, 20.7f, 57.38f), Vector3(-27.19f, -43.4f, 0.0f), 10.0f);
		track->AddWaypoint(Vector3(-152.0f, -5.9f, -83.24f), Vector3(-8.13f, 66.58f, 0.0f), 10.0f);
		track->AddWaypoint(Vector3(-183.0f, 2.829f, -47.45f), Vector3(-14.92f, 191.9f, 0.0f), 6.0f);
		track->AddWaypoint(Vector3(-199.8f, -12.81f, 4.214f), Vector3(-4.912f, 270.4f, 0.0f), 6.0f);
		track->AddWaypoint(Vector3(-33.74f, -9.024f, 0.5379f), Vector3(-7.222f, 283.0f, 0.0f), 10.0f);
		track->AddWaypoint(Vector3(0.0f, 0.0f, 10.0f), Vector3(-45.0f, 360.0f, 0.0f), 4.0f);
		return track;
	}

	Track<SceneNode>* BuildCompoundPatrol(SceneNode* target) {
		auto track = new Track<SceneNode>(target, 0.5f, true, false);
		track->AddWaypoint(Vector3(-10.0f, -15.4f,-10.0f), Vector3(0.0f, 0.0f, 0.0f));
		track->AddWaypoint(Vector3(-10.0f, -15.4f, 10.0f), Vector3(0.0f, 0.0f, 0.0f), 5.0f);
		track->AddWaypoint(Vector3(-10.0f, -15.4f, 10.0f), Vector3(0.0f, 90.0f, 0.0f));
		track->AddWaypoint(Vector3(10.0f, -15.4f, 10.0f), Vector3(0.0f, 90.0f, 0.0f), 5.0f);
		track->AddWaypoint(Vector3(10.0f, -15.4f, 10.0f), Vector3(0.0f, 180.0f, 0.0f));
		track->AddWaypoint(Vector3(10.0f, -15.4f,-10.0f), Vector3(0.0f, 180.0f, 0.0f), 5);
		track->AddWaypoint(Vector3(10.0f, -15.4f,-10.0f), Vector3(0.0f, 270.0f, 0.0f));
		track->AddWaypoint(Vector3(-10.0f, -15.4f, -10.0f), Vector3(0.0f, 270.0f, 0.0f), 5);
		track->AddWaypoint(Vector3(-10.0f, -15.4f, -10.0f), Vector3(0.0f, 360.0f, 0.0f));
		track->Start();
		return track;
	}

	Track<SceneNode>* BuildBottomRightPatrol(SceneNode* target) {
		auto track = new Track<SceneNode>(target, 0.5f, true, false);
		track->AddWaypoint(Vector3(67.3f, -15.4f, 38.92f), Vector3(0.0f, 58.5f, 0.0f));
		track->AddWaypoint(Vector3(175.0f, -15.4f, 105.0f), Vector3(0.0f, 58.5f, 0.0f), 20.0f);
		track->AddWaypoint(Vector3(175.0f, -15.4f, 105.0f), Vector3(0.0f, 238.5f, 0.0f));
		track->AddWaypoint(Vector3(67.3f, -15.4f, 38.92f), Vector3(0.0f, 238.5f, 0.0f),20.0f);
		track->AddWaypoint(Vector3(67.3f, -15.4f, 38.92f), Vector3(0.0f, 58.5f, 0.0f));
		track->Start();
		return track;
	}
	Track<SceneNode>* BuildTopRightPatrol(SceneNode* target) {
		auto track = new Track<SceneNode>(target, 0.5f, true, false);
		track->AddWaypoint(Vector3(68.34f, -15.4f, -61.13f), Vector3(0.0f, 149.1f, 0.0f));
		track->AddWaypoint(Vector3(148.4f, -15.4f, -187.3f), Vector3(0.0f, 149.1f, 0.0f), 20.0f);
		track->AddWaypoint(Vector3(148.4f, -15.4f, -187.3f), Vector3(0.0f, -30.9f, 0.0f));
		track->AddWaypoint(Vector3(68.34f, -15.4f, -61.13f), Vector3(0.0f, -30.9f, 0.0f), 20.0f);
		track->AddWaypoint(Vector3(68.34f, -15.4f, -61.13f), Vector3(0.0f, 149.1f, 0.0f));
		track->Start();
		return track;
	}
	Track<SceneNode>* BuildLeftPatrol(SceneNode* target) {
		auto track = new Track<SceneNode>(target, 0.5f, true, false);
		track->AddWaypoint(Vector3(-62.03f, -15.4f, 3.223f), Vector3(0.0f, -90.0f, 0.0f));
		track->AddWaypoint(Vector3(-178.1f, -15.4f, 3.518f), Vector3(0.0f, -90.0f, 0.0f), 20.0f);
		track->AddWaypoint(Vector3(-178.1f, -15.4f, 3.518f), Vector3(0.0f,  90.0f, 0.0f));
		track->AddWaypoint(Vector3(-62.03f, -15.4f, 3.223f), Vector3(0.0f,  90.0f, 0.0f), 20.0f);
		track->AddWaypoint(Vector3(-62.03f, -15.4f, 3.223f), Vector3(0.0f, -90.0f, 0.0f));
		track->Start();
		return track;
	}
}