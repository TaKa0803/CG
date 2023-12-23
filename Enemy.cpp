#include "Enemy.h"

void Enemy::Initialize(const Vector3&position) {
	IGameObject::Initialize("player");
	model_->IsEnableTexture(false);
	model_->SetBlendMode(BlendMode::kNone);
	world_.translate_ = position;
}

void Enemy::Update() {
	IGameObject::Update();
}

void Enemy::Draw(const Matrix4x4&viewP) {
	IGameObject::Draw(viewP);
}
