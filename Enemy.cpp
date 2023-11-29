#include "Enemy.h"
#include"TextureManager.h"
#include"imgui.h"


//Enemy::Enemy() {}

Enemy::~Enemy() {
	delete ehead_;
	delete eLA_;
	delete eRA_;

}

void Enemy::Initialize(int num, WorldTransform& world) {
	
	sikibetu = num;

	eh = TextureManager::LoadTex("resources/head.png");
	eWeapon = TextureManager::LoadTex("resources/Eweapon.png");

	ehead_ = Model::CreateFromOBJ("head");
	eLA_ = Model::CreateFromOBJ("Eweapon");
	eRA_ = Model::CreateFromOBJ("Eweapon");

	eWorld_ = world;
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
		ImGui::Text("HP : %d", hp_);
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
	if (isDraw_) {
		ehead_->Draw(ehT_.matWorld_, VP, eh);
		eLA_->Draw(eLT_.matWorld_, VP, eWeapon);
		eRA_->Draw(eRT_.matWorld_, VP, eWeapon);
	}
}

void Enemy::CollisionATK() {
	if (--hp_ <= 0) {
		isEDead_ = true;
	}
}

void Enemy::SetStart() {
	eWorld_ = stW_;
	ehT_ = stEhT_;
	eLT_ = stELT_;
	eRT_ = stERT_;

	isEDead_ = false;
	isFlying_ = false;
	isDraw_ = true;

	futtobiCount = 0;

	ehead_->SetAlpha(1);
	eLA_->SetAlpha(1);
	eRA_->SetAlpha(1);

	hp_ = 3;
}

void Enemy::SetStartData() {
	//初期情報保存
	stW_ = eWorld_;
	stEhT_ = ehT_;
	stELT_ = eLT_;
	stERT_ = eRT_;
}

bool Enemy::IsHitATKNum(const int num) {
	

	return false;
}

void Enemy::DeadAnimation(const WorldTransform&world) {


	if (!isEDead_) {
		isFlying_ = false;
	}

	else {

		if (!isFlying_) {
			isFlying_ = true;
			Vector3 pos = eWorld_.GetMatWorldTranslate();

			//初期化処理
			Vector3 ppos= eWorld_.GetParent().GetMatWorldTranslate();

			
			eWorld_.translate_ = pos;

			eWorld_.SetParent();

			eWorld_.UpdateMatrix();

			//吹っ飛び方向処理
			
			Vector3 hitpos = world.GetMatWorldTranslate();

			huttobi = pos - hitpos;

			huttobi = Normalize(huttobi);

			huttobi *= flypower;

			huttobi.y = +0.1f;

			futtobiCount = 0;
		}
		else {
			//更新処理
			eWorld_.translate_ += huttobi;

			float t = (float)futtobiCount / (float)maxFuttobiCount;

			t = 1.0f - t;

			ehead_->SetAlpha(t);
			eLA_->SetAlpha(t);
			eRA_->SetAlpha(t);

			if (futtobiCount++ >= maxFuttobiCount) {
				isDraw_ = false;
			}
		}

	}

}
