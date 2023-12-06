#include "DebugScene.h"
#include"TextureManager.h"
#include"imgui.h"

#include"DirectXFunc.h"
#include"function.h"

#include"RandomNum.h"

Particle MakeNewParticle(const Vector3& spawnPos, const Vector3& emiterSize, const Vector3& maxVelo, const Vector3& minVelo,const Vector4& colorMin, const Vector4& colorMax) {
	Particle ans;

	ans.position = spawnPos + RandomNumber::Get(-emiterSize / 2,emiterSize / 2);
	

	ans.velocity = RandomNumber::Get(minVelo, maxVelo);

	ans.color = RandomNumber::Get(colorMin, colorMax);

	return ans;
}



void DebugScene::Initialize() {
	model_ = Model::CreateFromOBJ("fence/fence");
	//texture = TextureManager::LoadTex("resources/fence/fence.png");

	//model_ = Model::CreateFromOBJ("ALPlayer");
	texture = TextureManager::LoadTex("resources/uvChecker.png");

	camera_.Initialize();
	camera_.SetTarget(&world_);

	sprite_ = new Sprite();
	//sprite_ = Sprite::CreateInstancing(texture, { 128,128 },kNuminstancing);
	sprite_ = Sprite::CreateInstancing(texture, { 256,256 },kNuminstancing+5);
	Vector3 center={ 640, 360,0};

	Vector3 velo = { 50,50,50 };

	Vector4 color = { 1,1,1,1 };
	Vector4 colormin = { 0,0,0,0 };

	for (uint32_t index = 0; index < (uint32_t)kNuminstancing; ++index) {
		insPos[index] = MakeNewParticle(center, { 0,0,0 }, -velo, velo,colormin,color);
	}

	
}

void DebugScene::Update() {
	world_.DrawDebug("box");
	camera_.DrawDebugWindow("camera");

	model_->DebugParameter("box");
	sprite_->DrawDebugImGui("sprite");

	for (uint32_t index = 0; index < (uint32_t)kNuminstancing; ++index) {
		insPos[index].position +=insPos[index].velocity*kDeltaTime;
		sprite_->SetPosition(&insPos[index].position);
	}

	world_.UpdateMatrix();
	camera_.Update();

	
}

void DebugScene::Draw() {
	model_->Draw(world_.matWorld_, camera_.GetViewProjectionMatrix(), texture);

	sprite_->DrawInstancing();
	//sprite_->Draw();
}

void DebugScene::Finalize() {
	delete model_;
	delete sprite_;
}
