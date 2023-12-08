#pragma once
#include"Vector4.h"
#include"Vector3.h"
#include"Matrix.h"
#include"WorldTransform.h"

struct Particle {
	WorldTransform world_;
	Vector3 velocity;
	Vector4 color;
	float lifeTime;
	float currentTime;
};

struct Particle4GPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;

};

Particle MakeNewParticle(const Vector3& spawnPos, const Vector3& emiterSize, const Vector3& maxVelo, const Vector3& minVelo, const Vector4& colorMin, const Vector4& colorMax, const Vector2& lifeTimeminXmaxY);