#pragma once
#include"WorldTransform/WorldTransform.h"

class Camera {

public:

	Camera();
	~Camera();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// デバッグウィンドウ表示
	/// </summary>
	/// <param name="name">ウィンドウ名</param>
	void DrawDebugWindow(const char* name);

	/// <summary>
	/// マトリックス更新
	/// </summary>
	void UpdateMatrixes();
public:

	/// <summary>
	/// カメラが注目しているものの座標取得
	/// </summary>
	/// <returns>translateを返却</returns>
	const Vector3& GetFeaturedPos()const { return CameraMotionSupport_.translate_; }

	/// <summary>
	/// メインカメラのWorldTransform取得
	/// </summary>
	/// <returns>WorldTransformを返却</returns>
	const WorldTransform& GetMainCamera()const { return mainCamera_; }

	/// <summary>
	/// ViewProjectionを取得
	/// </summary>
	/// <returns>ViewProjectionの4x4行列を返却</returns>
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjection_; }

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	float GetPCameraR_Y() { return CameraMotionSupport_.rotate_.y; }
public:
	void SetTarget(const WorldTransform* parent);

	void SetCameraDirection(const float far);

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

	//
	float FarZ = 1000.0f;

	//ビュー
	Matrix4x4 view_ = MakeIdentity4x4();

	//プロジェクション
	Matrix4x4 projection_ = MakeIdentity4x4();

	//VP
	Matrix4x4 viewProjection_ = MakeIdentity4x4();
};