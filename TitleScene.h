#pragma once
#include "IScene.h"
#include"Input.h"


class TitleScene : public IScene {

public:

	TitleScene();

	~TitleScene();

	void Initialize() override;

	void Update() override;

	void Draw() override;



private:
	//キー入力
	Input* input_=nullptr;

};

