#include "Enemy.h"

void Enemy::Initialize(const Vector3&position) {
	InstancingGameObject::Initialize("player");
	//model_->IsEnableTexture(false);
	//model_->SetBlendMode(BlendMode::kNone);
	world_.translate_ = position;
}

void Enemy::Update() {
	InstancingGameObject::Update();
}

void Enemy::Draw(const Matrix4x4& view) {
}
