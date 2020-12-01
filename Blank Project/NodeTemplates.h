#pragma once
#include "../nclgl/Vector3.h"

class SceneNode;
class Shader;

namespace Templates {

	SceneNode* Hut();
	SceneNode* Compound();
	SceneNode* CompoundStationarySoldiers();
	SceneNode* BarrelStack();
	SceneNode* StreetLight(Vector3 position = {}, Vector3 rotation = {});
}