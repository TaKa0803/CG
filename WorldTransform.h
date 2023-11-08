#pragma once
#include"struct.h"
#include"Matrix.h"

class WorldTransform {

public:

	void UpdateMatrix();

	Vector3 translate_ = { 0.0f,0.0f,0.0f };

	Vector3 rotate_ = { 0.0f,0.0f,0.0f };

	Vector3 scale_ = { 1.0f,1.0f,1.0f };

	Matrix4x4 matWorld_=MakeIdentity4x4();

	
	//ローカル座標取得
	const Vector3& GetTranslate() { return translate_; }

	//ローカル回転量取得
	const Vector3& GetRotate() { return rotate_; }

	//ローカルサイズ取得
	const Vector3& GetScale() { return scale_; }

	//matWorldの座標取得
	const Vector3& GetMatWorldTranslate() {
		return {
			matWorld_.m[3][0],
			matWorld_.m[3][1],
			matWorld_.m[3][2]
		};
	}

	void SetParent(const WorldTransform* parent) { parent_ = parent; }
private:
	const WorldTransform* parent_ = nullptr;


};