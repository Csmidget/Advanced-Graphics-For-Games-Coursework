#pragma once

#include <string>
#include "Mesh.h"

class HeightMap : public Mesh {
public:
	HeightMap(const std::string& name);
	~HeightMap(void) {};

	Vector3 GetHeightMapSize() { return size; }

private:
	Vector3 size;
};