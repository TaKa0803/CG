#include "Enemy.h"
#include"TextureManager.h"
#include"imgui.h"


Enemy::~Enemy() {
	delete ehead_;
	delete eLA_;
	delete eRA_;

}

void Enemy::Initialize(WorldTransform& world) {

	eh = TextureManager::LoadTex("resources/head.png");
	eWeapon = TextureManager::LoadTex("resources/Eweapon.png");

	ehead_ = Model::CreateFromOBJ("head");
	eLA_ = Model::CreateFromOBJ("Eweapon");
	eRA_ = Model::CreateFromOBJ("Eweapon");

	eWorld_.translate_.y = 2;

	ehT_.SetParent(&eWorld_);
	eLT_.SetParent(&ehT_);
	eRT_.SetParent(&ehT_);


	eLT_.translate_ = { -2.0f, 0, 0 };
	eRT_.translate_ = { 2.0f, 0, 0 };
	eRT_.rotate_.x = 3.14f / 2.0f;
}

void Enemy::Update() {
#pragma region 敵

	if (ImGui::BeginMenu("enemy")) {
		ImGui::DragFloat3("pos", &eWorld_.translate_.x, 0.01f);
		ImGui::EndMenu();
	}
	float mtheta = 1.0f / 60.0f * 3.14f;
	eWorld_.rotate_.y += mtheta;

	Vector3 move = { 0, 0, 0.1f };

	move = TransformNormal(move, eWorld_.matWorld_);
	// 光度さけす
	move.y = 0;

	eWorld_.translate_ += move;



	float theta = (1.0f / 60.0f) * 3.14f;



	eLT_.rotate_.x += theta;
	eRT_.rotate_.x += theta;


	eWorld_.UpdateMatrix();
	ehT_.UpdateMatrix();
	eLT_.UpdateMatrix();
	eRT_.UpdateMatrix();
#pragma endregion
}

void Enemy::Draw(const Matrix4x4& VP) {
	if (!isEDead_) {
		ehead_->Draw(ehT_.matWorld_, VP, eh);
		eLA_->Draw(eLT_.matWorld_, VP, eWeapon);
		eRA_->Draw(eRT_.matWorld_, VP, eWeapon);
	}
}
