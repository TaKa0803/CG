#pragma once
#include"Input.h"
#include"Model.h"
#include"struct.h"
#include"WorldTransform.h"
#include<iostream>

#include"Camera.h"

struct AABB {
	Vector3 min;
	Vector3 max;
};



class InGame {
public:
	void Initialize();

	void Update();

	void Draw();

	void Finalize();
private:
	Input* input = nullptr;

	enum PlayerState {
		kStay,
		kFalling
	};

	PlayerState pState_ = kStay;

	float gravity = -0.1f;

	//プレイヤー
	Model* playerM_ = nullptr;
	int playertexture;
	WorldTransform playerW_;
	const float pSize_ = 1;
	Vector3 GetmatT() { return {playerW_.matWorld_.m[3][0],playerW_.matWorld_.m[3][1],playerW_.matWorld_.m[3][2] }; }
	int nowParent = 0;
	//カメラ情報
	WorldTransform cameraTransform;
	//カメラの親（基本的にこちらでカメラ操作
	WorldTransform parentCmaera;

	const Vector3 startPos = { 0.0f,5.0f,0.0f };
	
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
	float planeSize = 10.0f;

	Vector3 GetmatPW1() { 
		return { 
		planeTrans1_.matWorld_.m[3][0],
		planeTrans1_.matWorld_.m[3][1] ,
		planeTrans1_.matWorld_.m[3][2]
	}; }
	Vector3 GetmatPW2() { return { planeTrans2_.matWorld_.m[3][0],planeTrans2_.matWorld_.m[3][1] ,planeTrans2_.matWorld_.m[3][2] }; }
	Vector3 GetmatPW3() { return { planeTrans3_.matWorld_.m[3][0],planeTrans3_.matWorld_.m[3][1] ,planeTrans3_.matWorld_.m[3][2] }; }


	int skydomeTex;
	Model* skydome_ = nullptr;
	WorldTransform skydomeTrans_;
	int scaleNum = 100;

	


	enum MoveState {
		kMoveLeft,
		kMoveRight
	};
	MoveState state_ = kMoveLeft;

	float moveSpd_ = 0.1f;

	float maxX_ = 10.0f;

	void PlaneUpdate();

	void Collision();


	int eh;
	int eWeapon;
	

	Model* ehead_ = nullptr;
	Model* eLA_ = nullptr;
	Model* eRA_ = nullptr;

	WorldTransform eWorld_;
	int eSize_ = 1;
	WorldTransform ehT_;
	WorldTransform eLT_;
	WorldTransform eRT_;

	Vector3 GetEmat() {
		return{
			eWorld_.matWorld_.m[3][0],
			eWorld_.matWorld_.m[3][1],
			eWorld_.matWorld_.m[3][2],
		};
	}



	int goalTex;
	Model* goal_ = nullptr;
	WorldTransform goalT_;

	int gSize_ = 1;

	Vector3 GetgoalMat() {
		return{
			goalT_.matWorld_.m[3][0],
			goalT_.matWorld_.m[3][1],
			goalT_.matWorld_.m[3][2]
		};
	}
	
	Camera camera;
};