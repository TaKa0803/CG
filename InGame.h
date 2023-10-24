#pragma once
#include"Input.h"
#include"Model.h"
#include"struct.h"
#include"WorldTransform.h"

class InGame {
public:
	void Initialize();

	void Update();

	void Draw();

private:
	Input* input = nullptr;

	//プレイヤー
	Model* playerM_ = nullptr;
	int playertexture;
	WorldTransform playerW_;
	

	//カメラ情報
	WorldTransform cameraTransform;
	//カメラの親（基本的にこちらでカメラ操作
	WorldTransform parentCmaera;

	
	//カメラ用
	Matrix4x4 VP;

	//地面
	int planeTex_;
	
	Model* plane1 = nullptr;
	WorldTransform planeTrans1_;

	Model* plane2 = nullptr;
	WorldTransform planeTrans2_;

	Model* plane3 = nullptr;
	WorldTransform planeTrans3_;

};