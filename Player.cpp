#include "Player.h"

#include"TextureManager.h"
#include<imgui.h>


Player::Player() {

}

Player::~Player() {
	IGameObject::~IGameObject();
}

void Player::Initialize() {
	IGameObject::Initialize("ALPlayer");
}

void Player::Update() {




	IGameObject::Update();
}

void Player::Draw(const Matrix4x4& viewprojection) {
	//IGameObject::Draw(viewprojection);
	model_->Draw(world_.matWorld_, viewprojection, TextureManager::uvChecker_);
}

void Player::DebugWindow(const char* name) {
	ImGui::Begin(name);
	world_.DrawDebug(name);
	model_->DebugParameter(name);
	ImGui::End();
}
