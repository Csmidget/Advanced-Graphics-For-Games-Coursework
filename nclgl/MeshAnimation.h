#pragma once
#include <vector>
#include <string>

#include "Matrix4.h"

class MeshAnimation
{
public:
	MeshAnimation();
	MeshAnimation(const std::string& filename);
	~MeshAnimation();

	unsigned int GetJointCount() const {
		return jointCount;
	}

	unsigned int GetFrameCount() const {
		return frameCount;
	}

	float GetFrameRate() const {
		return frameRate;
	}

	const Matrix4* GetRelativeJointData(unsigned int frame) const;
	const Matrix4* GetJointData(unsigned int frame) const;
	void GenerateRelativeJoints(const Matrix4* invBindPose);

protected:
	unsigned int	jointCount;
	unsigned int	frameCount;
	float			frameRate;

	std::vector<Matrix4>		allJoints;
	std::vector<Matrix4>		allRelativeJoints;
};

