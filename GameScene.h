#pragma once
#include "IScene.h"

#include<iostream>

#include"Input.h"
#include"Camera.h"
#include"Player.h"


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
	Player*player_=nullptr;

};