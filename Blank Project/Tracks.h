#pragma once
#include "Track.h"

class Camera;
class SceneNode;

namespace Tracks {

	Track<Camera>* BuildCameraTrack(Camera* cam);
	Track<SceneNode>* BuildCompoundPatrol(SceneNode* target);
	Track<SceneNode>* BuildBottomRightPatrol(SceneNode* target);
	Track<SceneNode>* BuildTopRightPatrol(SceneNode* target);
	Track<SceneNode>* BuildLeftPatrol(SceneNode* target);
}