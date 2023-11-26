#pragma once

#include"Sprite.h"

#include"Enemy.h"
#include<numbers>
#include<list>
#include<memory>
#include"Camera.h"
class LockOn {

public:

	~LockOn();

	void Initialize();

	void Update(Camera*camera);

	void Draw();

	void LockOnEnemy(const std::list<Enemy*>& enemies,Camera* camera);

	void SetBase(const WorldTransform* base) { base_ = base; }
private:
	//ロック音対象を選択
	void SerchEnemy();

	//画像
	Sprite* lockOn_;

	//基準点
	const WorldTransform* base_;

	const Enemy* target_ = nullptr;

	float kDigreeToRadian = ((float)std::numbers::pi / 180.0f);

	//最小距離
	float minDistance_ = 1.0f;
	//最大距離
	float maxDistance_ = 30.0f;

	float angleRange_ = 40.0f * kDigreeToRadian;


	
};

