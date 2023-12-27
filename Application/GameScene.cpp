#include "GameScene.h"
#include<imgui.h>

#include"InstancingModelManager/InstancingModelManager.h"


GameScene::GameScene() {
	input_ = Input::GetInstance();

	camera_ = std::make_unique<Camera>();
	player_ = std::make_unique<Player>();

	plane_ = std::make_unique<Plane>();
	
	enemyPopManager_ = std::make_unique<EnemyPopManager>();
	enemyPopManager_->LoadPopdata();
}

GameScene::~GameScene() {

}

void GameScene::Initialize() {
	//初期化
	camera_->Initialize();
	player_->Initialize();
	plane_->Initialize();

	//各種設定
	camera_->SetTarget(&player_->GetWorld());
	player_->SetCamera(camera_.get());
	enemyPopManager_->SetPlayerWorld(&player_->GetWorld());
}

void GameScene::Update() {
	//デバッグウィンドウ表示
	DebugWindows();

	//プレイヤー更新
	player_->Update();

	//カメラ更新
	camera_->Update();
	
	//
	enemyPopManager_->Update();

	//敵の生成処理
	std::unique_ptr<Enemy>newEnemy = std::make_unique<Enemy>();
	if (newEnemy = enemyPopManager_->PopEnemy()) {
		enemies_.push_back(std::move(newEnemy));
	}

	for (auto& enemy : enemies_) {
		enemy->Update();
	}

}

void GameScene::Draw() {
	plane_->Draw(camera_->GetViewProjectionMatrix());
	
	for (auto& enemy : enemies_) {
		enemy->Draw(camera_->GetViewProjectionMatrix());
	}

	player_->Draw(camera_->GetViewProjectionMatrix());

	InstancingModelManager::GetInstance()->DrawAllModel(camera_->GetViewProjectionMatrix());
}

void GameScene::DebugWindows() {
	//カメラのデバッグ表示
	camera_->DrawDebugWindow("camera");

	//プレイヤーデバッグ表示
	player_->DebugWindow("player");
	
	plane_->DebagWindow();
}
