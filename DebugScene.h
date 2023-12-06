#pragma once
#include"Model.h"
#include"WorldTransform.h"
#include"Camera.h"
#include"Sprite.h"


class DebugScene {
public:
	void Initialize();

	void Update();

	void Draw();

	void Finalize();

private:

	Camera camera_;

	Model* model_;
	WorldTransform world_;
	int texture;

	Sprite* sprite_;

	static const int kNuminstancing = 10;

	

	Particle insPos[kNuminstancing];

	const float kDeltaTime = 1.0f / 60.0f;
};

