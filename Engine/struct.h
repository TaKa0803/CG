#pragma once
#include<stdint.h>
#include<vector>
#include<string>

#include"Math/Vector2.h"
#include"Math/Vector3.h"
#include"Math/Vector4.h"
#include"Math/Matrix.h"


#pragma region 構造体

struct VertexData {
	Vector4 position = { 0,0,0,0 };
	Vector2 texcoord = { 0,0 };
	Vector3 normal = { 0,0,0 };
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};





struct AABB {
	Vector3 minV;
	Vector3 maxV;
};





struct Material {
	Vector4 color;
		
	Matrix4x4 uvTransform;	   
	
	int32_t enableLighting;
	//float padding[3];

	int32_t enableTexture;
	//float padding2[3];
	
	int32_t enableHalfLambert;
	//float padding3[3];
	
	float discardNum;
	//float padding4[3];

	
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
	//float padding2[3];
};





#pragma endregion
