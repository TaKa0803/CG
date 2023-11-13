#pragma once
#include"Model.h"
#include"WorldTransform.h"
#include"Input.h"
#include"Camera.h"
#include<optional>

class Player {

public:

	~Player();

	void Initialize();

	void Update();

	void Draw();

	const WorldTransform& GetWorld() { return playerW_; }

	int GetNowParent() { return nowParent; }

	void SetCamera(const Camera& camera) { camera_ = &camera; }

	const AABB GetAABB() {
		AABB pAABB = {
		.min{GetmatT().x - pSize_,GetmatT().y - pSize_,GetmatT().z - pSize_},
		.max{GetmatT().x + pSize_,GetmatT().y + pSize_,GetmatT().z + pSize_}
		};

		return pAABB;
	};


	void OnCollision(int hitparent, const WorldTransform* parent);

	void NoCollision();

	void SetStartPosition();
private:

#pragma region 

	void StayInitialize();

	void FallInitialize();

	void DashInitialize();

	void ATKInitialize();

	void FallUpdate();

	void StayUpdate();

	void DashUpdate();

	void ATKUpdate();

#pragma endregion



	
private:

	Input* input_ = nullptr;

	enum class PlayerState {
		kStay,
		kFalling,
		kATK,
		kDash
	};


	PlayerState pState_ = PlayerState::kStay;

	std::optional<PlayerState>stateRequest_ = std::nullopt;

	float gravity = -0.1f;


	//プレイヤー
	Model* playerM_ = nullptr;
	int playertexture;

	WorldTransform playerW_;

	const float pSize_ = 1;
	Vector3 GetmatT() { return { playerW_.matWorld_.m[3][0],playerW_.matWorld_.m[3][1],playerW_.matWorld_.m[3][2] }; }
	
	int nowParent = 0;

	const Camera* camera_=nullptr;

	const Vector3 startPos = { 0.0f,5.0f,0.0f };

	//ダッシュ量
	float dashPower_ = 1;

	float dashtimeSecond_ = 0.1f;

	//ダッシュの長さ (秒)*(フレーム)
	float dashTime;

	int count_ = 0;

	//ダッシュ量
	Vector3 dashVelo;

	//減速量
	Vector3 DecelerationVector;


	//ダッシュの遅延
	float dashSecond_ = 1;
};