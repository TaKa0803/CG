#pragma once
#include"WorldTransform.h"
#include"Model.h"


class Enemy {
public:

	~Enemy();

	void Initialize(WorldTransform& world);

	void Update();

	void Draw(const Matrix4x4& VP);


	const WorldTransform& GetWorld() { return eWorld_; }

	const float GetSize() { return eSize_; }

	void SetDead(bool isdead) { isEDead_ = isdead; }

	bool GetDead() { return isEDead_; }

	void SetParent(const WorldTransform* parent) { eWorld_.SetParent(parent); }
private:

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

};

