#pragma once
#include"struct.h"

class WorldTransform {

public:

	void UpdateMatrix();

	Vector3 translate_ = { 0.0f,0.0f,0.0f };

	Vector3 rotate_ = { 0.0f,0.0f,0.0f };

	Vector3 scale_ = { 1.0f,1.0f,1.0f };

	Matrix4x4 matWorld_;

	WorldTransform* parent_ = nullptr;
private:


};