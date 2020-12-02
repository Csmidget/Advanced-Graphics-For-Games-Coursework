#pragma once
#include "../nclgl/Vector3.h"

class SceneNode;
class Shader;

namespace Prefabs {

	SceneNode* Hut();
	SceneNode* Compound();

	//These have to be done seperately to the compound as the compound is static,
	//animated objects like soldiers need to be dynamic to cast shadows correctly.
	SceneNode* CompoundStationarySoldiers();

	SceneNode* BarrelStack();
	SceneNode* StreetLight(Vector3 position = {}, Vector3 rotation = {});
}