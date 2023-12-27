#include "Player.h"

#include"TextureManager/TextureManager.h"
#include<imgui.h>


Player::Player() {
	input_ = Input::GetInstance();
}

Player::~Player() {
	
}

void Player::Initialize() {
	InstancingGameObject::Initialize("player");
}

void Player::Update() {
	//移動処理
	Move();
	//行列更新
	InstancingGameObject::Update();
}

void Player::Draw(const Matrix4x4& viewprojection) {
	//IGameObject::Draw(viewprojection);
	//model_->Draw(world_.matWorld_, viewprojection, TextureManager::uvChecker_);
}

void Player::DebugWindow(const char* name) {
	ImGui::Begin(name);
	world_.DrawDebug(name);
	//model_->DebugParameter(name);
	ImGui::End();
}



void Player::Move() {
	
	//移動取得
	Vector3 move = input_->GetWASD();

	//カメラ方向に向ける
	move = TransformNormal(move, camera_->GetMainCamera().matWorld_);

	move.y = 0;

	if (move != Vector3(0, 0, 0)) {
		world_.rotate_.y = GetYRotate({ move.x,move.z });
	}
	//加算
	world_.translate_ += move;
}
