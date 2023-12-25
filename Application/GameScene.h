#pragma once
#include "IScene/IScene.h"

#include<iostream>
#include<list>

#include"Input/Input.h"
#include"Camera/Camera.h"
#include"Player.h"
#include"Plane.h"
#include"Enemy.h"
#include"EnemyPopManager.h"


class GameScene : public IScene {

public:

	GameScene();

	~GameScene();

	void Initialize() override;

	void Update() override;

	void Draw() override;

private:
	void DebugWindows();

private:
	//キー入力
	Input* input_ = nullptr;
	//カメラクラス
	std::unique_ptr<Camera> camera_;

	///以下ゲーム実装
	std::unique_ptr<Player>player_;

	std::unique_ptr<Plane>plane_;

	std::list<std::unique_ptr<Enemy>>enemies_;

	std::unique_ptr<EnemyPopManager>enemyPopManager_;
};