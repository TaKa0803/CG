#include "GameObject.h"

GameObject::~GameObject() {
	delete model_;
}

void GameObject::Initialize(const std::string& filepath) {
	model_ = Model::CreateFromOBJ(filepath);
}

void GameObject::Update() {
	world_.UpdateMatrix();
}

void GameObject::Draw(const Matrix4x4& viewProjection) {
	model_->Draw(world_.matWorld_, viewProjection);
}

