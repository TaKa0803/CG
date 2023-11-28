#pragma once

#include"Sprite.h"
#include"Input.h"
#include"Enemy.h"
#include<numbers>
#include<list>
#include<memory>
#include"Camera.h"
class LockOn {

public:

	~LockOn();

	void Initialize();

	void Update(const std::list<Enemy*>& enemies,Camera*camera);

	void Draw();

	void LockOnEnemy(const std::list<Enemy*>& enemies,Camera* camera);

	void SetBase(const WorldTransform* base) { base_ = base; }

	void Reset();

	const Enemy* GetTarget()const { return target_; }

	void SetAutoLockOn(bool set) { isAutoLockOn_ = set; }
	const bool GetAutoLockOn()const { return isAutoLockOn_; }
private:
	
	Input* input_ = nullptr;

	//画像
	Sprite* lockOn_;

	//基準点
	const WorldTransform* base_;

	const Enemy* target_ = nullptr;

	float kDigreeToRadian = ((float)std::numbers::pi / 180.0f);

	//最小距離
	float minDistance_ = 5.0f;
	//最大距離
	float maxDistance_ = 20.0f;

	float angleRange_ = 45.0f * kDigreeToRadian;

	bool isAutoLockOn_ = false;

	
};

