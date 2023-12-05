#include "DebugScene.h"
#include"TextureManager.h"
#include"imgui.h"

#include"DirectXFunc.h"
#include"function.h"

void DebugScene::Initialize() {
	model_ = Model::CreateFromOBJ("fence/fence");
	//texture = TextureManager::LoadTex("resources/fence/fence.png");

	//model_ = Model::CreateFromOBJ("ALPlayer");
	texture = TextureManager::LoadTex("resources/uvChecker.png");

	camera_.Initialize();
	camera_.SetTarget(&world_);

	sprite_ = new Sprite();
	//sprite_ = Sprite::CreateInstancing(texture, { 128,128 },kNuminstancing);
	sprite_ = Sprite::CreateInstancing(texture, { 128,128 },kNuminstancing);
	sprite_->SetPosition({ 640, 360});

	for (uint32_t index = 0; index < (uint32_t)kNuminstancing; ++index) {
		insPos[index] = { (float)index * 5.0f,(float)index * 6.0f,(float)index*0.1f};
	}

	
}

void DebugScene::Update() {
	world_.DrawDebug("box");
	camera_.DrawDebugWindow("camera");

	model_->DebugParameter("box");
	sprite_->DrawDebugImGui("sprite");



	world_.UpdateMatrix();
	camera_.Update();

	
}

void DebugScene::Draw() {
	model_->Draw(world_.matWorld_, camera_.GetViewProjectionMatrix(), texture);

	sprite_->DrawInstancing(-1,insPos);
	//sprite_->Draw();
}

void DebugScene::Finalize() {
	delete model_;
	delete sprite_;
}
