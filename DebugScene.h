#pragma once
#include"Model.h"
#include"WorldTransform.h"
#include"Camera.h"
#include"Sprite.h"

#include"Particle.h"
#include"ParticleEmiter.h"

#include<memory>

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

	ParticleEmiter*pE_;

	Particle insPos[kNuminstancing];

	const float kDeltaTime = 1.0f / 60.0f;


	Vector3 center = { 640, 360,0 };

	Vector3 velo = { 50,50,50 };

	Vector3 minvelo = { -50,-50,0 };

	Vector4 color = { 1,1,1,1 };
	Vector4 colormin = { 0,0,0,1 };

	bool checkUpdate_ = false;
};

