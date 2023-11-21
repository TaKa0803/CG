#include "DebugScene.h"
#include"TextureManager.h"
#include"imgui.h"

void DebugScene::Initialize() {
	model_ = Model::CreateFromOBJ("fence/fence");
	texture = TextureManager::LoadTex("resources/fence/fence.png");

	camera_.Initialize();
	camera_.SetTarget(&world_);
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
