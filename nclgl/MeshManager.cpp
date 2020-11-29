#include "MeshManager.h"
#include "Mesh.h"
#include "MeshAnimation.h"
#include "MeshMaterial.h"

std::map<std::string, Mesh*> MeshManager::loadedMeshes;
std::map<std::string, MeshMaterial*> MeshManager::loadedMeshMaterials;
std::map<std::string, MeshAnimation*> MeshManager::loadedMeshAnimations;

Mesh* MeshManager::LoadMesh(std::string meshName) {

	if (loadedMeshes.find(meshName) != loadedMeshes.end())
		return loadedMeshes[meshName];

	Mesh* mesh = Mesh::LoadFromMeshFile(meshName);

	loadedMeshes.emplace(meshName, mesh);

	return mesh;
}

MeshAnimation* MeshManager::LoadMeshAnimation(std::string meshAnimationName) {

	if (loadedMeshAnimations.find(meshAnimationName) != loadedMeshAnimations.end())
		return loadedMeshAnimations[meshAnimationName];

	MeshAnimation* meshAnim = new MeshAnimation(meshAnimationName);

	loadedMeshAnimations.emplace(meshAnimationName, meshAnim);

	return meshAnim;
}

MeshMaterial* MeshManager::LoadMeshMaterial(std::string meshMaterialName) {

	if (loadedMeshMaterials.find(meshMaterialName) != loadedMeshMaterials.end())
		return loadedMeshMaterials[meshMaterialName];

	MeshMaterial* meshMat = new MeshMaterial(meshMaterialName);

	loadedMeshMaterials.emplace(meshMaterialName, meshMat);

	return meshMat;
}

void MeshManager::Cleanup() {
	for (auto m : loadedMeshes) {
		delete m.second;
	}
	loadedMeshes.clear();

	for (auto m : loadedMeshAnimations) {
		delete m.second;
	}
	loadedMeshAnimations.clear();

	for (auto m : loadedMeshMaterials) {
		delete m.second;
	}
	loadedMeshMaterials.clear();
}