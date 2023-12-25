#include "Enemy.h"

void Enemy::Initialize(const Vector3&position) {
	GameObject::Initialize("player");
	model_->IsEnableTexture(false);
	model_->SetBlendMode(BlendMode::kNone);
	world_.translate_ = position;
}

void Enemy::Update() {
	GameObject::Update();
}

void Enemy::Draw(const Matrix4x4&viewP) {
	GameObject::Draw(viewP);
}
