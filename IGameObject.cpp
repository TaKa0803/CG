#include "IGameObject.h"

IGameObject::~IGameObject() {
	delete model_;
}

void IGameObject::Initialize(const std::string& filepath) {
	model_ = Model::CreateFromOBJ(filepath);
}

void IGameObject::Update() {
	world_.UpdateMatrix();
}

void IGameObject::Draw(const Matrix4x4& viewProjection) {
	model_->Draw(world_.matWorld_, viewProjection);
}

