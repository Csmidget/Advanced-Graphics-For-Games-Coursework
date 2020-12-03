/*
Class:Vector4
Implements:
Author:Rich Davison
Description:VERY simple Vector4 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
#pragma once

#include "Vector3.h"

class Vector4	{
public:
	Vector4(void) {
		x = y = z = w = 1.0f;
	}
	Vector4(float x, float y, float z, float w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	Vector3 ToVector3() {
		return Vector3(x, y, z);
	}

	inline void operator+=(const Vector4& a) {
		x += a.x;
		y += a.y;
		z += a.z;
		w += a.w;
	}

	inline void operator-=(const Vector4& a) {
		x -= a.x;
		y -= a.y;
		z -= a.z;
		w -= a.w;
	}

	Vector4 Normalised() const {
		Vector4 n = *this;
		n.Normalise();
		return n;
	}

	void	Normalise() {
		float length = Length();

		if (length != 0.0f) {
			length = 1.0f / length;
			x = x * length;
			y = y * length;
			z = z * length;
			w = w * length;
		}
	}

	static Vector4 Lerp(Vector4 from, Vector4 to, float progress) {
		Vector4 retVec{};
		retVec.x = (1 - progress) * from.x + progress * to.x;
		retVec.y = (1 - progress) * from.y + progress * to.y;
		retVec.z = (1 - progress) * from.z + progress * to.z;
		retVec.w = (1 - progress) * from.w + progress * to.w;
		return retVec;
	}

	float			Length() const {
		return sqrt((x * x) + (y * y) + (z * z) + (w * w));
	}

	inline Vector4  operator*(const float a) const {
		return Vector4(x * a, y * a, z * a, w * a);
	}

	~Vector4(void){}

	float x;
	float y;
	float z;
	float w;
};

