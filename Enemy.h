#pragma once
#include"WorldTransform.h"
#include"Model.h"


class Enemy {
public:

	//Enemy();
	~Enemy();

	void Initialize(int num,WorldTransform& world);

	void Update();

	void Draw(const Matrix4x4& VP);

	void CollisionATK();

	void SetStart();

	void SetStartData();

	//吹っ飛びアニメーション
	void DeadAnimation(const WorldTransform& world);

public:

	const WorldTransform& GetWorld() const{ return eWorld_; }

	const float GetSize()const { return eSize_; }

	bool GetDead()const { return isEDead_; }

	bool IsHitATKNum(const int num);

	int GetSikibetuNum()const { return sikibetu; }
public:
	
	void SetDead(bool isdead) { isEDead_ = isdead; }

	void SetParent(const WorldTransform* parent) { eWorld_.SetParent(parent); }
private:

	int sikibetu;


	int eh;
	int eWeapon;


	Model* ehead_ = nullptr;
	Model* eLA_ = nullptr;
	Model* eRA_ = nullptr;

	WorldTransform eWorld_;
	float eSize_ = 1.5f;
	WorldTransform ehT_;
	WorldTransform eLT_;
	WorldTransform eRT_;

	bool isEDead_ = false;

	int hp_ = 3;

	//描画フラグ
	bool isDraw_ = true;

	
	//吹っ飛び初期化
	bool isFlying_ = false;

	//初期情報保存
	WorldTransform stW_;
	WorldTransform stEhT_;
	WorldTransform stELT_;
	WorldTransform stERT_;

	float flypower = 0.2f;

	Vector3 huttobi;

	const int maxFuttobiCount = 30;
	int futtobiCount = 0;
};

