#include"WorldTransform.h"
#include"Math/Matrix.h"
#include<imgui.h>

Matrix4x4 WorldTransform::UpdateMatrix() {
	matWorld_ = MakeAffineMatrix(scale_, rotate_, translate_);

	if (parent_) {
		matWorld_ = matWorld_* parent_->matWorld_;
	}

	return matWorld_;
}

void WorldTransform::DrawDebug(const char*name) {
#ifdef _DEBUG

	ImGui::Begin(name);
	ImGui::DragFloat3("pos", &translate_.x, 0.01f);
	ImGui::DragFloat3("rotate", &rotate_.x, 0.01f);
	ImGui::DragFloat3("scale", &scale_.x, 0.01f);
	ImGui::End();

#endif // _DEBUG
}
