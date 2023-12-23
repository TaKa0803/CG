#pragma once
#include"IGameObject.h"
#include"Input.h"
#include"Camera.h"

class Player :public IGameObject {

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

	const Camera* camera_;
};