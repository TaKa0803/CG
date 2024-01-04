#include "InstancingGameObject.h"

InstancingGameObject::~InstancingGameObject() {
	delete model_;
}

void InstancingGameObject::Initialize(const std::string& filepath) {
	model_ = InstancingModel::Coreate(filepath);
}

void InstancingGameObject::Update() {
	world_.UpdateMatrix();
}

void InstancingGameObject::Draw() {
	model_->Draw(world_);
}
