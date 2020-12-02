#pragma once

#include <map>
#include <string>

class Mesh;
class MeshMaterial;
class MeshAnimation;

class MeshManager {

	friend class OGLRenderer;

	static std::map<std::string, Mesh*> loadedMeshes;
	static std::map<std::string, MeshMaterial*> loadedMeshMaterials;
	static std::map<std::string, MeshAnimation*> loadedMeshAnimations;
	static void	Cleanup();

public:
	static Mesh*	LoadMesh(std::string meshName);
	static MeshAnimation* LoadMeshAnimation(std::string meshAnimationName);
	static MeshMaterial* LoadMeshMaterial(std::string meshMaterialName);
};