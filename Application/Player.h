#pragma once
#include"Instancing/InstancingGameObject.h"
#include"Input/Input.h"
#include"Camera/Camera.h"

class Player :public InstancingGameObject {

public:
	Player();
	~Player();

	void Initialize();

	void Update();

	void Draw(const Matrix4x4&viewprojection);

	void DebugWindow(const char*name);

	void SetCamera(Camera* camera) { camera_ = camera; }
private:
	//移動
	void Move();


private:
	Input* input_ = nullptr;

	const Camera* camera_=nullptr;
};