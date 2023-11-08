#include"Camera.h"
#include"WinApp.h"

void Camera::Initialize() {

	//カメラの初期距離を設定
	mainCamera_.translate_.z = rangeCameraFeaturedPoint;

	//親子関係の処理
	mainCamera_.SetParent(&CameraMotionSupport_);


	//各種更新に必要な処理
	view_ = Inverse(mainCamera_.matWorld_);
	projection_ = MakePerspectiveFovMatrix(0.45f, (float)WinApp::kClientWidth / (float)WinApp::kClientHeight, 0.1f, 100.0f);
	viewProjection_ = Multiply(view_, projection_);
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
			CameraMotionSupport_.matWorld_ = Multiply(CameraMotionSupport_.matWorld_, FeaturedWorldTransform_->matWorld_);
		}
	}


	//メインカメラの更新
	mainCamera_.UpdateMatrix();

	//各種更新に必要な処理
	view_ = Inverse(mainCamera_.matWorld_);
	projection_ = MakePerspectiveFovMatrix(0.45f, (float)WinApp::kClientWidth / (float)WinApp::kClientHeight, 0.1f, 100.0f);
	viewProjection_ = Multiply(view_, projection_);
}

void Camera::SetTarget(const WorldTransform* parent) {
	FeaturedWorldTransform_ = parent;
}

void Camera::SetCameraFar(const float farZ) {
	mainCamera_.translate_.z = farZ;
}
