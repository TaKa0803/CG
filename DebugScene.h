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

	static const int kNuminstancing = 30;

	ParticleEmiter*pE_;

	Particle insPos[kNuminstancing];

	const float kDeltaTime = 1.0f / 60.0f;


	Vector3 center = { 0, 0,0 };

	Vector3 maxvelo = { 0.5f,0.5f,0 };

	Vector3 minvelo = { -0.5f,-0.5f,0 };

	MinMaxDataV3 velo = {
		.minData{minvelo},
		.maxData{maxvelo}
	};

	Vector4 colormax = { 1,1,1,1 };
	Vector4 colormin = { 0,0,0,1 };

	MinMaxDataV4 color = {
		.minData{colormax},
		.maxData{colormin}
	};

	

	bool checkUpdate_ = false;
};

