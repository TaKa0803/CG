#include "DebugScene.h"
#include"TextureManager.h"
#include"imgui.h"

void DebugScene::Initialize() {
	model_ = Model::CreateFromOBJ("fence/fence");
	//texture = TextureManager::LoadTex("resources/fence/fence.png");
	
	//model_ = Model::CreateFromOBJ("ALPlayer");
	texture = TextureManager::LoadTex("resources/uvChecker.png");

	camera_.Initialize();
	camera_.SetTarget(&world_);

	sprite_ = new Sprite();
	sprite_=Sprite::
}

void DebugScene::Update() {
	world_.DrawDebug("box");
	camera_.DrawDebugWindow("camera");

	model_->DebugParameter("box");

	world_.UpdateMatrix();
	camera_.Update();
}

void DebugScene::Draw() {
	model_->Draw(world_.matWorld_, camera_.GetViewProjectionMatrix(), texture);
}

void DebugScene::Finalize() {
	delete model_;
}
