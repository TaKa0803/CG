#pragma once
#include<stdint.h>
#include<vector>
#include<string>

#include"Vector2.h"
#include"Vector3.h"
#include"Matrix.h"


#pragma region 構造体

struct AABB {
	Vector3 minV;
	Vector3 maxV;
};

struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	int32_t enableTexture;
	float padding2[3];
	int32_t enableHalfLambert;
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};





#pragma endregion
