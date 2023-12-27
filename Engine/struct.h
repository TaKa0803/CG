#pragma once
#include<stdint.h>
#include<vector>
#include<string>

#include"Math/Vector2.h"
#include"Math/Vector3.h"
#include"Math/Vector4.h"
#include"Math/Matrix.h"


#pragma region 構造体

enum class BlendMode {
	//!<ブレンドなし
	kNone,
	//!<通常αブレンド
	kNormal,
	//!<加算
	kAdd,
	//!<減算
	kSubtract,
	//!<乗算
	kMultily,
	//!<スクリーン
	kScreen,
	//利用してはいけない
	kCountOfBlendMode
};

//塗りつぶし設定
enum class FillMode {
	//埋める
	kSolid,
	//ワイヤー
	kWireFrame,
	//使用不可
	kCountOfFillMode
};

struct WorldTransformation {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

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
