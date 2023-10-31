#include"InGame.h"
#include"TextureManager.h"
#include"externals/Imgui/imgui.h"
#include<cmath>
#include<iostream>


//AABB同士
bool InCollision(const AABB& a, const AABB& b) {

	if ((a.min.x <= b.max.x && a.max.x >= b.min.x) &&//x軸
		(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
		(a.min.z <= b.max.z && a.max.z >= b.min.z)
		) {
		return true;
	}
	return false;
}



void InGame::Initialize() {
	input = Input::GetInstance();


	//カメラ傾き
	parentCmaera.rotate_ = { 0.6f,0.0f,0.0f };
	cameraTransform.translate_ = { 0.0f,0.0f,-50.0f };

	cameraTransform.parent_ = &parentCmaera;

	playerM_ = Model::CreateFromOBJ("ALPlayer.obj");
	playerM_->IsEnableShader(false);

	playertexture = TextureManager::LoadTex("resources/player.png");

	plane1 = Model::CreateFromOBJ("Plane.obj");
	plane2 = Model::CreateFromOBJ("Plane.obj");
	plane3 = Model::CreateFromOBJ("Plane.obj");

	planeTex_ = TextureManager::LoadTex("resources/Plane.png");

	skydome_ = Model::CreateFromOBJ("sssssk.obj");
	//skydome_->IsEnableShader(false);
	skydomeTex = TextureManager::LoadTex("resources/Space.png");




	planeTrans1_.translate_ = { 0.0f,-2.0f,0.0f };
	planeTrans2_.translate_ = { 0.0f,-2.0f,planeSize * 2.0f };
	planeTrans3_.translate_ = { 0.0f,-2.0f,planeSize * 4.0f };


	eh = TextureManager::LoadTex("resources/head.png");
	eWeapon = TextureManager::LoadTex("resources/Eweapon.png");

	ehead_ = Model::CreateFromOBJ("head.obj");
	eLA_ = Model::CreateFromOBJ("Eweapon.obj");
	eRA_ = Model::CreateFromOBJ("Eweapon.obj");

	ehead_->IsEnableShader(false);
	eLA_->IsEnableShader(false);
	eRA_->IsEnableShader(false);

	goalTex = TextureManager::LoadTex("resources/goal.png");
	goal_ = Model::CreateFromOBJ("goal.obj");
	goal_->IsEnableShader(false);
	goalT_.parent_ = &planeTrans3_;


	goalT_.translate_.y = 2;

	eWorld_.translate_.y = 2;

	eWorld_.parent_ = &planeTrans2_;
	ehT_.parent_ = &eWorld_;
	eLT_.parent_ = &ehT_;
	eRT_.parent_ = &ehT_;

	eLT_.translate_= { -2.0f, 0, 0 };
	eRT_.translate_ = { 2.0f, 0, 0 };
	eRT_.rotate_.x = 3.14f / 2.0f;;

}

void InGame::Update() {



#pragma region 台座処理
	ImGui::Begin("plane");
	ImGui::DragFloat3("pos1", &planeTrans1_.translate_.x, 0.01f);
	ImGui::DragFloat3("pos2", &planeTrans2_.translate_.x, 0.01f);
	ImGui::DragFloat3("pos3", &planeTrans3_.translate_.x, 0.01f);

	ImGui::DragFloat3("posr1", &planeTrans1_.rotate_.x, 0.01f);
	ImGui::DragFloat3("posr2", &planeTrans2_.rotate_.x, 0.01f);
	ImGui::DragFloat3("posr3", &planeTrans3_.rotate_.x, 0.01f);

	ImGui::End();

	PlaneUpdate();

	planeTrans1_.UpdateMatrix();
	planeTrans2_.UpdateMatrix();
	planeTrans3_.UpdateMatrix();

#pragma endregion

#pragma region プレイヤー挙動
	ImGui::Begin("obj");
	ImGui::DragFloat3("pos", &playerW_.translate_.x, 0.01f);
	ImGui::DragFloat3("rotate", &playerW_.rotate_.x, 0.01f);
	ImGui::DragFloat3("scale", &playerW_.scale_.x, 0.01f);
	ImGui::Text("%d", nowParent);
	ImGui::End();

	Vector3 moveVelo = { 0,0,0 };
	bool ismoveActive = false;

	float spd = 0.3f;

	if (input->IsControllerActive()) {
		Vector2 nyu = input->GetjoyStickL();

		if (nyu.x == 0 && nyu.y == 0) {
			moveVelo = { 0,0,0 };
		}else{
			moveVelo = { nyu.x,0,nyu.y };
			moveVelo = Normalize(moveVelo);
			moveVelo = Scalar(spd, moveVelo);

			Matrix4x4 C_Affine = MakeAffineMatrix(parentCmaera.scale_, parentCmaera.rotate_, parentCmaera.translate_);
			moveVelo = TransformNormal(moveVelo, C_Affine);
			moveVelo.y = 0;
		}
	}
	else {
		if (input->PushKey(DIK_UP)) {
			moveVelo.z += spd;
			ismoveActive = true;
		}
		if (input->PushKey(DIK_DOWN)) {
			moveVelo.z -= spd;
			ismoveActive = true;
		}
		if (input->PushKey(DIK_RIGHT)) {
			moveVelo.x += spd;
			ismoveActive = true;
		}
		if (input->PushKey(DIK_LEFT)) {
			moveVelo.x -= spd;
			ismoveActive = true;
		}
	}

	if (ismoveActive) {
		Matrix4x4 C_Affine = MakeAffineMatrix(parentCmaera.scale_, parentCmaera.rotate_, parentCmaera.translate_);
		moveVelo = TransformNormal(moveVelo, C_Affine);
		moveVelo.y = 0;



	}

	if (pState_ == kFalling) {
		moveVelo.y += gravity;
	}


	playerW_.translate_ = Add(playerW_.translate_, moveVelo);

	if (playerW_.translate_.y <= -50) {
		playerW_.translate_ = startPos;
		playerW_.parent_ = nullptr;
		pState_ = kStay;
		nowParent = 0;
	}

	playerW_.UpdateMatrix();
#pragma endregion

#pragma region カメラ処理
	ImGui::Begin("Camera");
	ImGui::DragFloat3("Camera translate", &parentCmaera.translate_.x, 0.01f);
	ImGui::DragFloat3("Camera rotate", &parentCmaera.rotate_.x, 0.01f);
	ImGui::End();

	if (input->IsControllerActive()) {
		Vector2 rota = input->GetjoyStickR();

		if (rota.x == 0 && rota.y == 0) {

		}
		else {
			Vector3 rotate_ = { -rota.y,rota.x,0 };

			rotate_ = Normalize(rotate_);

			//一回で動く量
			float theta = (1.0f / 120.0f) * 3.14f;

			rotate_ = Scalar(theta, rotate_);

			parentCmaera.rotate_ = Add(parentCmaera.rotate_, rotate_);

		}
	}

	//カメラをプレイヤーの座標に合わせる
	parentCmaera.translate_ = GetmatT();

	parentCmaera.UpdateMatrix();

	//カメラ処理
	cameraTransform.UpdateMatrix();


	//親の行列掛け算
	Matrix4x4 viewMatrix = Inverse(cameraTransform.matWorld_);
	//透視投影行列
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
	VP = Multiply(viewMatrix, projectionMatrix);


#pragma endregion

#pragma region 天球
	ImGui::Begin("skydome");
	ImGui::DragInt("scale", &scaleNum, 0.01f);
	ImGui::End();
	skydomeTrans_.scale_ = { (float)scaleNum,(float)scaleNum,(float)scaleNum };
	skydomeTrans_.UpdateMatrix();
#pragma endregion

#pragma region 敵
	
	ImGui::Begin("enemy");
	ImGui::DragFloat3("pos", &eWorld_.translate_.x, 0.01f);
	ImGui::End();

	float mtheta = 1.0f / 60.0f * 3.14f;
	eWorld_.rotate_.y += mtheta;

	Vector3 move = { 0, 0, 0.1f };

	move = TransformNormal(move, eWorld_.matWorld_);
	// 光度さけす
	move.y = 0;

	eWorld_.translate_ = Add(eWorld_.translate_, move);

	
	
	float theta = (1.0f / 60.0f) * 3.14f;



	eLT_.rotate_.x += theta;
	eRT_.rotate_.x += theta;


	eWorld_.UpdateMatrix();
	ehT_.UpdateMatrix();
	eLT_.UpdateMatrix();
	eRT_.UpdateMatrix();
#pragma endregion


#pragma region ゴール

	goalT_.UpdateMatrix();
#pragma endregion



	Collision();
}

void InGame::Draw() {

	playerM_->Draw(playerW_.matWorld_, VP, playertexture);


	plane1->Draw(planeTrans1_.matWorld_, VP, planeTex_);
	plane2->Draw(planeTrans2_.matWorld_, VP, planeTex_);
	plane3->Draw(planeTrans3_.matWorld_, VP, planeTex_);

	skydome_->Draw(skydomeTrans_.matWorld_, VP, skydomeTex);

	ehead_->Draw(ehT_.matWorld_, VP, eh);
	eLA_->Draw(eLT_.matWorld_, VP, eWeapon);
	eRA_->Draw(eRT_.matWorld_, VP, eWeapon);


	goal_->Draw(goalT_.matWorld_, VP, goalTex);
}

void InGame::PlaneUpdate() {

	switch (state_) {
	case InGame::kMoveLeft:
		planeTrans2_.translate_.x -= moveSpd_;

		if (planeTrans2_.translate_.x <= -maxX_) {
			planeTrans2_.translate_.x = -maxX_;
			state_ = kMoveRight;
		}

		break;
	case InGame::kMoveRight:
		planeTrans2_.translate_.x += moveSpd_;

		if (planeTrans2_.translate_.x >= maxX_) {
			planeTrans2_.translate_.x = maxX_;
			state_ = kMoveLeft;
		}
		break;
	default:
		break;
	}

}

void InGame::Collision() {
	AABB pAABB = {
		.min{GetmatT().x - pSize_,GetmatT().y - pSize_,GetmatT().z - pSize_},
		.max{GetmatT().x + pSize_,GetmatT().y + pSize_,GetmatT().z + pSize_}
	};

	AABB pParentP = {
		.min{GetmatT().x,GetmatT().y - pSize_,GetmatT().z},
		.max{GetmatT().x,GetmatT().y - pSize_,GetmatT().z}
	};

	AABB p1 = {
		.min{GetmatPW1().x - planeSize,GetmatPW1().y - 1.0f,GetmatPW1().z - planeSize},
		.max{GetmatPW1().x + planeSize,GetmatPW1().y + 1.0f,GetmatPW1().z + planeSize}
	};

	AABB p2 = {
		.min{GetmatPW2().x - planeSize,GetmatPW2().y - 1.0f,GetmatPW2().z - planeSize},
		.max{GetmatPW2().x + planeSize,GetmatPW2().y + 1.0f,GetmatPW2().z + planeSize}
	};
	AABB p3 = {
		.min{GetmatPW3().x - planeSize,GetmatPW3().y - 1.0f,GetmatPW3().z - planeSize},
		.max{GetmatPW3().x + planeSize,GetmatPW3().y + 1.0f,GetmatPW3().z + planeSize}
	};

	AABB goa = {
		.min{GetgoalMat().x - gSize_,GetgoalMat().y - gSize_,GetgoalMat().z - gSize_},
		.max{GetgoalMat().x + gSize_,GetgoalMat().y + gSize_,GetgoalMat().z + gSize_}
	};

	AABB ene = {
		.min{GetEmat().x - eSize_,GetEmat().y - eSize_,GetEmat().z - eSize_},
		.max{GetEmat().x + eSize_,GetEmat().y + eSize_,GetEmat().z + eSize_}
	};

	bool ishit = false;


	//土台1とのコリジョン
	if (InCollision(pAABB, p1)) {
		ishit = true;
		pState_ = kStay;


		//if (InCollision(p2, pParentP)) {
			//ペアレントが違うものだったら
		if (nowParent != 1) {
			nowParent = 1;
			//台座から見たプレイヤーの座標取得
			Vector3 pos = Subtract(GetmatT(), GetmatPW1());


			playerW_.translate_ = pos;
			playerW_.translate_.y = 1.0f + pSize_;
			playerW_.parent_ = &planeTrans1_;


			playerW_.UpdateMatrix();
		}
		//}
	}
	else if (InCollision(pAABB, p2)) {
		ishit = true;
		pState_ = kStay;

		//if (InCollision(p2, pParentP)) {
			//ペアレントが違うものだったら
		if (nowParent != 2) {
			nowParent = 2;
			//台座から見たプレイヤーの座標取得
			Vector3 pos = Subtract(GetmatT(), GetmatPW2());

			playerW_.translate_ = pos;
			playerW_.translate_.y = 1.0f + pSize_;
			playerW_.parent_ = &planeTrans2_;


			playerW_.UpdateMatrix();
		}
		//}
	}
	else if (InCollision(pAABB, p3)) {
		ishit = true;
		pState_ = kStay;


		//if (InCollision(p2, pParentP)) {
			//ペアレントが違うものだったら
		if (nowParent != 3) {
			nowParent = 3;
			//台座から見たプレイヤーの座標取得
			Vector3 pos = Subtract(GetmatT(), GetmatPW3());


			playerW_.translate_ = pos;
			playerW_.translate_.y = 1.0f + pSize_;
			playerW_.parent_ = &planeTrans3_;


			playerW_.UpdateMatrix();
		}
		//}
	}
	if (ishit == false) {
		//親子関係を消して処理
		playerW_.translate_ = GetmatT();
		playerW_.parent_ = nullptr;
		playerW_.UpdateMatrix();
		pState_ = kFalling;
		nowParent = 0;
	}


	if (InCollision(pAABB, goa) || InCollision(pAABB, ene)) {
		playerW_.translate_ = startPos;
		playerW_.parent_ = nullptr;
		pState_ = kStay;
		nowParent = 0;
	}

}
