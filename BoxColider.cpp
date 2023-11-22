#include "BoxColider.h"
#include"TextureManager.h"

BoxColider::~BoxColider() {
	delete model_;
}


void BoxColider::Initialize(const WorldTransform* world) {
	world_.SetParent(world);
	
	model_ = Model::CreateFromOBJ("ALPlayer");

	texture_ = TextureManager::LoadTex("resources/Plane.png");

}


void BoxColider::Update() {
	world_.UpdateMatrix();
}

void BoxColider::Draw(const Matrix4x4& VP) {
	model_->Draw(world_.matWorld_, VP, texture_);
}
