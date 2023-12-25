#include"Camera.h"
#include"WinApp/WinApp.h"
#include<imgui.h>

Camera::Camera() {}

Camera::~Camera() {}

void Camera::Initialize() {

	//カメラの初期距離を設定
	mainCamera_.translate_.z = rangeCameraFeaturedPoint;

	//ポイントカメラの回転の設定
	CameraMotionSupport_.rotate_ = { 0.6f,0.0f,0.0f };

	//親子関係の処理
	mainCamera_.SetParent(&CameraMotionSupport_);

	//行列更新
	CameraMotionSupport_.UpdateMatrix();
	mainCamera_.UpdateMatrix();

	//各種更新に必要な処理
	view_ = Inverse(mainCamera_.matWorld_);
	projection_ = MakePerspectiveFovMatrix(0.45f, (float)WindowApp::kClientWidth / (float)WindowApp::kClientHeight, 0.1f, FarZ);
	viewProjection_ = view_ * projection_;
}

void Camera::Update() {


	//注目するものがあるとき
	if (FeaturedWorldTransform_) {
		//座標のみ取得するフラグが起動しているとき
		if (isOnlyGetPosition) {
			CameraMotionSupport_.translate_ = FeaturedWorldTransform_->GetMatWorldTranslate();
			CameraMotionSupport_.UpdateMatrix();
		}
		else {
			CameraMotionSupport_.UpdateMatrix();
			//起動していないとき行列をかけて親子関係処理
			CameraMotionSupport_.matWorld_ = CameraMotionSupport_.matWorld_* FeaturedWorldTransform_->matWorld_;
		}
	}


	//メインカメラの更新
	mainCamera_.UpdateMatrix();

	//各種更新に必要な処理
	view_ = Inverse(mainCamera_.matWorld_);
	projection_ = MakePerspectiveFovMatrix(0.45f, (float)WindowApp::kClientWidth / (float)WindowApp::kClientHeight, 0.1f, FarZ);
	viewProjection_ = view_* projection_;
}

void Camera::DrawDebugWindow(const char* name) {
#ifdef _DEBUG

	if (ImGui::BeginMenu(name)) {
		ImGui::Text("mainCamera");
		ImGui::DragFloat3("mainC pos", &mainCamera_.translate_.x, 0.01f);		
		ImGui::DragFloat3("mainC rotate", &mainCamera_.rotate_.x, 0.01f);
		ImGui::DragFloat3("mainC scale", &mainCamera_.scale_.x, 0.01f);

		ImGui::Text("LocationCenterMotion");
		ImGui::DragFloat3("PCM pos", &CameraMotionSupport_.translate_.x, 0.01f);
		ImGui::DragFloat3("PCM rotate", &CameraMotionSupport_.rotate_.x, 0.01f);
		ImGui::DragFloat3("PCM scale", &CameraMotionSupport_.scale_.x, 0.01f);
		ImGui::Checkbox("isOnlyGetPosition", &isOnlyGetPosition);
		ImGui::EndMenu();
	}
	else {

		ImGui::Begin(name);
		ImGui::Text("mainCamera");
		ImGui::DragFloat2("mainC pos", &mainCamera_.translate_.x, 0.01f);
		ImGui::DragFloat("mainC farZ", &mainCamera_.translate_.z);
		ImGui::DragFloat3("mainC rotate", &mainCamera_.rotate_.x, 0.01f);
		ImGui::DragFloat3("mainC scale", &mainCamera_.scale_.x, 0.01f);

		ImGui::Text("LocationCenterMotion");
		ImGui::DragFloat3("PCM pos", &CameraMotionSupport_.translate_.x, 0.01f);
		ImGui::DragFloat3("PCM rotate", &CameraMotionSupport_.rotate_.x, 0.01f);
		ImGui::DragFloat3("PCM scale", &CameraMotionSupport_.scale_.x, 0.01f);
		ImGui::Checkbox("isOnlyGetPosition", &isOnlyGetPosition);
		ImGui::End();
	}
#endif // _DEBUG

}

void Camera::UpdateMatrixes() {
	//行列更新
	CameraMotionSupport_.UpdateMatrix();
	mainCamera_.UpdateMatrix();

	//各種更新に必要な処理
	view_ = Inverse(mainCamera_.matWorld_);
	projection_ = MakePerspectiveFovMatrix(0.45f, (float)WindowApp::kClientWidth / (float)WindowApp::kClientHeight, 0.1f, FarZ);
	viewProjection_ = view_* projection_;
}

void Camera::SetTarget(const WorldTransform* parent) {
	FeaturedWorldTransform_ = parent;
}

void Camera::SetCameraDirection(const float farZ) {
	mainCamera_.translate_.z = farZ;
}
