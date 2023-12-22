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

	Camera camera_;
	


};