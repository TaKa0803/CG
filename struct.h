#pragma once
#include<stdint.h>
#include<vector>
#include<string>

#include"Vector2.h"
#include"Vector3.h"
#include"Matrix.h"


#pragma region 構造体

struct Vector4 {
	float x;
	float y;
	float z;
	float w;

	Vector3 GetXYZ() { return{ x,y,z }; }

	friend Vector4 operator-(const Vector4& v) { return { -v.x,-v.y,-v.z,-v.w }; }
};

struct Particle {
	Vector3 position;
	Vector3 velocity;
	Vector4 color;
};

struct AABB {
	Vector3 minV;
	Vector3 maxV;
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
	float padding3[3];
	
	float discardNum;
	float padding4[3];

	
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float padding;
	float intensity;
	float padding2[3];
};





#pragma endregion
