#pragma once
#include"Input.h"
#include"Model.h"
#include"struct.h"
#include"WorldTransform.h"
#include<iostream>

#include"Camera.h"

#include"Player.h"

#include"Enemy.h"

#include"LockOn.h"

#include"EffectHit.h"

class InGame {
public:
	void Initialize();

	void Update();

	void Draw();

	void Finalize();
private:


	Input* input_ = Input::GetInstance();

	Vector3 from0 = Normalize(Vector3{ 1.0f,0.7f,0.5f });
	Vector3 to0 = -from0;

	Vector3 from1 = Normalize(Vector3{ -0.6f,0.9f,0.2f });
	Vector3 to1 = Normalize(Vector3{0.4f,0.7f,-0.5f});

	Matrix4x4 rotateM0 = DirectionToDirection(Normalize(Vector3{ 1.0f,0.0f,0.0f }), Normalize(Vector3{ -1.0f,0.0f,0.0f }));

	Matrix4x4 rotateM1 = DirectionToDirection(from0, to0);
	Matrix4x4 rotateM2 = DirectionToDirection(from1, to1);


};