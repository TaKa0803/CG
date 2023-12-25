#include "Plane.h"

void Plane::Initialize() {
	GameObject::Initialize("plane");

	world_.scale_ = { 500,500,500 };
	model_->SetUVScale({ 500,500,500 });
	world_.UpdateMatrix();
}

void Plane::DebagWindow() {
	model_->DebugParameter("plane");
}

void Plane::Draw(const Matrix4x4& viewp) {
	GameObject::Draw(viewp);
}
