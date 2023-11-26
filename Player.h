#pragma once
#include"Model.h"
#include"WorldTransform.h"
#include"Input.h"
#include"Camera.h"
#include"BoxColider.h"

#include<optional>
#include<array>
class Player {

public:

	~Player();

	void Initialize();

	void Update();

	void DashCameraUpdate(Camera& camera);


	void Draw();

	const WorldTransform& GetWorld() { return world_; }

	const WorldTransform& GetWeaponWorld() { return weaponW_; }

	const float GetWeaponRadius() { return WRadius_; }

	int GetNowParent() { return nowParent; }

	void SetCamera(const Camera& camera) { camera_ = &camera; }

	const AABB GetAABB() {
		AABB pAABB = {
		.minV{GetmatT().x - pSize_,GetmatT().y - pSize_,GetmatT().z - pSize_},
		.maxV{GetmatT().x + pSize_,GetmatT().y + pSize_,GetmatT().z + pSize_}
		};

		return pAABB;
	};


	void OnCollision(int hitparent, const WorldTransform* parent);

	void NoCollision();

	void SetStartPosition();


	bool IsStateATK() { if (pState_ == PlayerState::kATK) { return true; } else { return false; } }

	bool FallingCheck();

	const AABB& GetWeaponAABB() { return collider_->GetAABB(); }

private:

#pragma region 状態

	void StayInitialize();

	void FallInitialize();

	void DashInitialize();

	void ATKInitialize();

	void JumpInitialize();

	void FallUpdate();

	void StayUpdate();

	void DashUpdate();

	void ATKUpdate();

	void JumpUpdate();
#pragma endregion



	
private:

	Input* input_ = nullptr;

	enum class PlayerState {
		kStay,
		kFalling,
		kATK,
		kDash,
		kjump
	};


	PlayerState pState_ = PlayerState::kStay;

	std::optional<PlayerState>stateRequest_ = std::nullopt;

	float gravity = -0.1f;

	BoxColider *collider_;

	//プレイヤー
	Model* model_ = nullptr;
	int playertexture;

	WorldTransform world_;

	const float pSize_ = 1;
	Vector3 GetmatT() { return { world_.matWorld_.m[3][0],world_.matWorld_.m[3][1],world_.matWorld_.m[3][2] }; }
	
	int nowParent = 0;

	const Camera* camera_=nullptr;

	const Vector3 startPos = { 0.0f,5.0f,0.0f };

	//ダッシュ量
	float dashPower_ = 1;

	//ダッシュする秒数
	float dashtimeSecond_ = 0.1f;

	//ダッシュの長さ (秒)*(フレーム)
	float dashTime;

	int count_ = 0;

	//ダッシュ量
	Vector3 dashVelo;

	//減速量
	Vector3 DecelerationVector;


	
	//カメラの最初の地点取得
	Vector3 startCameraPos_;

	//ダッシュ時のカメラの遅延量
	float cameraDelaySecond_ = 0.5f;

	float delayT_ = 0;

	bool isCameraDelay_ = false;
	
#pragma region 攻撃

	void NextATK();

	//攻撃用定数
	struct ConstATK {
		//振りかぶりの時間
		uint32_t anicipationTime;
		//ための時間
		uint32_t chargeTime;
		//攻撃振りの時間
		uint32_t swingTime;
		//硬直時間
		uint32_t recoveryTime;
		//振りかぶりの移動速さ
		float anticipationSpeed;
		//ための移動速さ
		float chargeSpeed;
		//攻撃振りの移動速さ
		float swingSpeed;
	};
	//コンボ最大数
	static const int maxComboNum_ = 3;

	//コンボ構造体テーブル
	static const std::array<ConstATK, maxComboNum_>kConstATKs_;

	//攻撃用ワーク
	struct WorkATK {
		//攻撃ギミックの媒介変数
		uint32_t attackParameter = 0;
		//コンボ番号
		int32_t comboIndex=0;
		//コンボのフェーズ取得
		int32_t inComboPhase = 0;
		//トンボが次に進むか
		bool comboNext = false;;
	};

	WorkATK workATK_{};

	//武器のワールド座標
	WorldTransform weaponW_;
	//モデル
	Model* weaponM_ = nullptr;
	//武器画像
	int weaponTex_;

	float WRadius_ = 4;


	Vector3 weaponStR[maxComboNum_] = { {0.0f,0.0f,0.0f},{1.5f,-1.5f,0.0f},{1.5f,1.5f,0.0f} };
	Vector3 weaponEndR[maxComboNum_] = { { 1.9f,0.0f,0.0f },{1.5f,1.5f,0.0f},{1.5f,-1.5f,0.0f} };

	
	
#pragma endregion

#pragma region ジャンプ

	Vector3 velo_ = { 0.0f,0.0f,0.0f };

	const float jumpPower_ = 1.0f;

	//上がる速度が0になる時間
	float zeroSecond = 0.2f;



#pragma endregion


	

	const char* name = "Player";
	std::string keys[5] = {
		"Camera Dash Delay Second",
		"Dash Power",
		"Dash Second",
		"ATK Second"
	};
};