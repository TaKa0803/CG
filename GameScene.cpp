#include "GameScene.h"
#include<imgui.h>

GameScene::GameScene() {
	camera_ = std::make_unique<Camera>();

	player_ = new Player();
}

GameScene::~GameScene() {
	delete player_;
}


void GameScene::Initialize() {
	camera_->Initialize();
	player_->Initialize();
}

void GameScene::Update() {
	//デバッグウィンドウ表示
	DebugWindows();

	//カメラ更新
	camera_->Update();

	//プレイヤー更新
	player_->Update();
}

void GameScene::Draw() {
	player_->Draw(camera_->GetViewProjectionMatrix());

}

void GameScene::DebugWindows() {
	//カメラのデバッグ表示
	camera_->DrawDebugWindow("camera");

	//プレイヤーデバッグ表示
	player_->DebugWindow("player");
	
}
