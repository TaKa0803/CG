#pragma once
#include"WorldTransform.h"

class Camera {

public:

	void Initialize();

	void Update();

	void DrawDebugWindow(const char* name);

	void UpdateMatrixes();
public:

	const Vector3& GetFeaturedPos()const { return CameraMotionSupport_.translate_; }

	const WorldTransform& GetMainCamera()const { return mainCamera_; }

	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjection_; }

	const Matrix4x4& GetCameraDirectionToFace()const { return mainCamera_.matWorld_; }

	
	float GetPCameraR_Y() { return CameraMotionSupport_.rotate_.y; }
public:
	void SetTarget(const WorldTransform* parent);

	void SetCameraFar(const float far);

	void SetCameraFeaturedPos(const Vector3& pos) { CameraMotionSupport_.translate_ = pos; }

	void SetCameraR_Y(float y) { CameraMotionSupport_.rotate_.y = y; }
private:
	//カメラ本体
	WorldTransform mainCamera_;

	//カメラの注目点があるときの回転処理用
	WorldTransform CameraMotionSupport_;

	//注目しているWorldT
	const WorldTransform* FeaturedWorldTransform_ = nullptr;

	//カメラの注目品との初期距離
	const float rangeCameraFeaturedPoint = -50;

	//完全追従するか座標のみ取得するか
	bool isOnlyGetPosition = true;

	//ビュー
	Matrix4x4 view_ = MakeIdentity4x4();

	//プロジェクション
	Matrix4x4 projection_ = MakeIdentity4x4();

	//VP
	Matrix4x4 viewProjection_ = MakeIdentity4x4();
};