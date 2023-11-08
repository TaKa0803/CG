#pragma once
#include"WorldTransform.h"

class Camera {

public:

	void Initialize();

	void Update();

	void IsDebugWindowActive(bool isActive);


public:

	const Matrix4x4& GetViewProjectionMatrix() { return viewProjection_; }

public:
	void SetParent(const WorldTransform* parent);

	void SetCameraFar(const float far);

private:
	//カメラ本体
	WorldTransform mainCamera_;

	//カメラの注目点があるときの回転処理用
	WorldTransform CameraMotionSupport_;

	//注目しているWorldT
	const WorldTransform* FeaturedWorldTransform_=nullptr;

	//カメラの注目品との初期距離
	const float rangeCameraFeaturedPoint = -50;

	//完全追従するか座標のみ取得するか
	bool isOnlyGetPosition = true;

	Matrix4x4 view_=MakeIdentity4x4();

	Matrix4x4 projection_=MakeIdentity4x4();

	Matrix4x4 viewProjection_ = MakeIdentity4x4();
};