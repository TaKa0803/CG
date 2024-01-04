#pragma once
#include<stdint.h>
#include<vector>
#include<string>

#include"Math/Vector2.h"
#include"Math/Vector3.h"
#include"Math/Vector4.h"
#include"Math/Matrix.h"
#include"Quaternion/Quaternion.h"

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


struct AnimationVertex {
	std::vector<Vector3>startPositions;//開始時の頂点
	std::vector<Vector3>endPositions;//終了時の頂点
};

struct WorldTransformation {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct BoneVertexData {
	int IndexID;	//影響を与えるインデックスの番号
	float Weight;	//影響度
};

struct BoneData {
	std::vector<BoneVertexData>data;//影響する頂点の影響データ
};

struct NodeTransformation {
	std::string name;
	Vector3 translate;
	Quaternion rotate;
	Vector3 scale;
};


struct AnimationData {
	std::string name;			//名前
	float duration;				//継続時間
	uint32_t boneNum;				//ボーン数
	std::vector<NodeTransformation>bones_;//アニメーション後のボーンデータ
};


struct VertexData {
	uint32_t indexID;
	Vector4 position = { 0,0,0,0 };
	Vector2 texcoord = { 0,0 };
	Vector3 normal = { 0,0,0 };
	
};

struct MaterialData {
	std::string textureFilePath;
};

struct Node {
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node>children;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;

	Node rootNode;
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
