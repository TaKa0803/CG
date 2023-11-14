#include "Player.h"
#include"TextureManager.h"
#include<imgui.h>
#include"GlobalVariables.h"

Player::~Player() {
	delete playerM_;
	delete weaponM_;
}

void Player::Initialize() {
	input_ = Input::GetInstance();

	playerM_ = Model::CreateFromOBJ("ALPlayer");
	playerM_->IsEnableShader(false);
	playerW_.UpdateMatrix();
	playertexture = TextureManager::LoadTex("resources/player.png");


	weaponM_ = Model::CreateFromOBJ("weapon/weapon");
	weaponTex_ = TextureManager::LoadTex("resources/weapon/weapon.png");

	weaponW_.SetParent(&playerW_);


#pragma region globalVariables
	GlobalVariables* globalV = GlobalVariables::GetInstance();
	
	globalV->CreateGroup(name);


	globalV->AddItem(name,keys[0], cameraDelaySecond_);
	globalV->AddItem(name, keys[1], dashPower_);
	globalV->AddItem(name, keys[2], dashtimeSecond_);
	globalV->AddItem(name, keys[3], MovingSecond_);


	cameraDelaySecond_= globalV->GetFloatvalue(name, keys[0]);
	dashPower_ =globalV->GetFloatvalue(name, keys[1]);
	dashtimeSecond_= globalV->GetFloatvalue(name, keys[2]);
	MovingSecond_ = globalV->GetFloatvalue(name, keys[3]);
#pragma endregion

}

void Player::Update() {
#pragma region プレイヤー挙動
	GlobalVariables* globalV = GlobalVariables::GetInstance();
	cameraDelaySecond_ = globalV->GetFloatvalue(name, keys[0]);
	dashPower_ = globalV->GetFloatvalue(name, keys[1]);
	dashtimeSecond_ = globalV->GetFloatvalue(name, keys[2]);
	MovingSecond_ = globalV->GetFloatvalue(name, keys[3]);

	if (ImGui::BeginMenu("player")) {
		ImGui::DragFloat3("pos", &playerW_.translate_.x, 0.01f);
		ImGui::DragFloat3("rotate", &playerW_.rotate_.x, 0.01f);
		ImGui::DragFloat3("scale", &playerW_.scale_.x, 0.01f);
		ImGui::Text("%d", nowParent);
		ImGui::EndMenu();
	}

	if (stateRequest_) {
		pState_ = stateRequest_.value();
		switch (pState_) {
		case Player::PlayerState::kStay:
			StayInitialize();
			break;
		case Player::PlayerState::kFalling:
			FallInitialize();
			break;
		case Player::PlayerState::kATK:
			ATKInitialize();
			break;
		case Player::PlayerState::kDash:
			DashInitialize();
			break;
		default:
			break;
		}
		stateRequest_ = std::nullopt;
	}

	switch (pState_) {
	case Player::PlayerState::kStay:
		StayUpdate();
		break;
	case Player::PlayerState::kFalling:
		FallUpdate();
		break;
	case Player::PlayerState::kATK:
		ATKUpdate();
		break;
	case Player::PlayerState::kDash:
		DashUpdate();
		break;
	default:
		break;
	}


	//落ちたら初期位置へ
	if (playerW_.translate_.y <= -50) {
		SetStartPosition();
	}

	playerW_.UpdateMatrix();
#pragma endregion
}

Vector3 Esing(const Vector3& st, const Vector3 ed, float t) {
	return{
		st.x * (1.0f - t) + ed.x * t,
		st.y * (1.0f - t) + ed.y * t,
		st.z * (1.0f - t) + ed.z * t,
	};
}


void Player::DashCameraUpdate(Camera& camera) {
	

	if (isCameraDelay_) {
		delayT_ += 1.0f / (cameraDelaySecond_ * 60);

		
			Vector3 newpos = Esing(startCameraPos_, camera.GetFeaturedPos(), delayT_);

			camera.SetCameraFeaturedPos(newpos);
		
			camera.UpdateMatrixes();
			if (delayT_ >= 1.0f) {
				isCameraDelay_ = false;
			}
	}
	
}

void Player::Draw() {
	playerM_->Draw(playerW_.matWorld_, camera_->GetViewProjectionMatrix(), playertexture);

	if (pState_ == PlayerState::kATK) {
		weaponM_->Draw(weaponW_.matWorld_, camera_->GetViewProjectionMatrix(), weaponTex_);
	}
}

void Player::OnCollision(int hitparent, const WorldTransform* parent) {

	if (pState_ == PlayerState::kFalling) {
		stateRequest_ = PlayerState::kStay;
	}
	//if (InCollision(p2, pParentP)) {
			//ペアレントが違うものだったら
	if (nowParent != hitparent) {
		nowParent = hitparent;
		//台座から見たプレイヤーの座標取得
		Vector3 pos = Subtract(GetmatT(), parent->GetMatWorldTranslate());


		playerW_.translate_ = pos;
		playerW_.translate_.y = 1.0f + pSize_;
		playerW_.SetParent(parent);


		playerW_.UpdateMatrix();
	}
}

void Player::NoCollision() {
	//親子関係を消して処理
	playerW_.translate_ = GetmatT();
	playerW_.SetParent();
	playerW_.UpdateMatrix();

	stateRequest_ = PlayerState::kFalling;
	nowParent = 0;
}

void Player::SetStartPosition() {
	playerW_.translate_ = startPos;
	playerW_.SetParent();
	stateRequest_ = PlayerState::kStay;
	nowParent = 0;
}

void Player::StayInitialize() {
}

void Player::FallInitialize() {
}

void Player::DashInitialize() {
	Vector3 moveVelo = { 0,0,0 };
	bool ismoveActive = false;

	if (input_->IsControllerActive()) {
		Vector2 nyu = input_->GetjoyStickL();

		if (nyu.x == 0 && nyu.y == 0) {
			moveVelo = { 0,0,0 };
		}
		else {
			moveVelo = { nyu.x,0,nyu.y };
			moveVelo = Normalize(moveVelo);
			moveVelo = Scalar(dashPower_, moveVelo);

			Matrix4x4 C_Affine = camera_->GetCameraDirectionToFace();
			moveVelo = TransformNormal(moveVelo, C_Affine);
			moveVelo.y = 0;
			bool ismoveActive = true;
		}
	}
	else {
		if (input_->PushKey(DIK_UP)) {
			moveVelo.z += 1;
			bool ismoveActive = true;
		}
		if (input_->PushKey(DIK_DOWN)) {
			moveVelo.z -= 1;
			bool ismoveActive = true;
		}
		if (input_->PushKey(DIK_RIGHT)) {
			moveVelo.x += 1;
			bool ismoveActive = true;
		}
		if (input_->PushKey(DIK_LEFT)) {
			moveVelo.x -= 1;
			bool ismoveActive = true;
		}

	}
	if (ismoveActive) {
		if (moveVelo.x == 0 && moveVelo.z) {
			moveVelo = Normalize(moveVelo);
			moveVelo = Scalar(dashPower_, moveVelo);
		}
		Matrix4x4 C_Affine = camera_->GetCameraDirectionToFace();
		moveVelo = TransformNormal(moveVelo, C_Affine);
		moveVelo.y = 0;
	}

	dashTime = dashtimeSecond_ * 60;

	count_ = 0;

	//速度渡し
	dashVelo = moveVelo;

	//減速度設定
	DecelerationVector = {

		-(dashVelo.x / dashTime),
		-(dashVelo.y / dashTime),
		-(dashVelo.z / dashTime)

	};

	delayT_ = 0;
	isCameraDelay_ = true;

	startCameraPos_ = camera_->GetFeaturedPos();
}

void Player::ATKInitialize() {
	animationT_ = 0;
}

float CheckR_F_Y(const Vector2& v) { return std::atan2(v.x, v.y); }


void Player::FallUpdate() {
	Vector3 moveVelo = { 0,0,0 };
	bool ismoveActive = false;

	float spd = 0.3f;

	if (input_->IsControllerActive()) {
		Vector2 nyu = input_->GetjoyStickL();

		if (nyu.x == 0 && nyu.y == 0) {
			moveVelo = { 0,0,0 };
		}
		else {
			moveVelo = { nyu.x,0,nyu.y };
			moveVelo = Normalize(moveVelo);
			moveVelo = Scalar(spd, moveVelo);

			Matrix4x4 C_Affine = camera_->GetCameraDirectionToFace();
			moveVelo = TransformNormal(moveVelo, C_Affine);
			moveVelo.y = 0;
		}
	}
	else {
		if (input_->PushKey(DIK_UP)) {
			moveVelo.z += spd;
			ismoveActive = true;
		}
		if (input_->PushKey(DIK_DOWN)) {
			moveVelo.z -= spd;
			ismoveActive = true;
		}
		if (input_->PushKey(DIK_RIGHT)) {
			moveVelo.x += spd;
			ismoveActive = true;
		}
		if (input_->PushKey(DIK_LEFT)) {
			moveVelo.x -= spd;
			ismoveActive = true;
		}
	}

	if (ismoveActive) {
		Matrix4x4 C_Affine = camera_->GetCameraDirectionToFace();
		moveVelo = TransformNormal(moveVelo, C_Affine);
		moveVelo.y = 0;

		Vector2 newR = { moveVelo.x,moveVelo.z };
		playerW_.rotate_.y = CheckR_F_Y(newR);
	}

	moveVelo.y += gravity;

	playerW_.translate_ = Add(playerW_.translate_, moveVelo);

}

void Player::StayUpdate() {

#pragma region 移動

	bool ismoveActive = false;

	float spd = 0.3f;
	Vector3 moveVelo = { 0,0,0 };
	if (input_->IsControllerActive()) {
		Vector2 nyu = input_->GetjoyStickL();

		if (nyu.x == 0 && nyu.y == 0) {
			moveVelo = { 0,0,0 };
		}
		else {
			moveVelo = { nyu.x,0,nyu.y };
			moveVelo = Normalize(moveVelo);
			moveVelo = Scalar(spd, moveVelo);

			Matrix4x4 C_Affine = camera_->GetCameraDirectionToFace();
			moveVelo = TransformNormal(moveVelo, C_Affine);
			moveVelo.y = 0;
		}
	}
	else {
		if (input_->PushKey(DIK_UP)) {
			moveVelo.z += spd;
			ismoveActive = true;
		}
		if (input_->PushKey(DIK_DOWN)) {
			moveVelo.z -= spd;
			ismoveActive = true;
		}
		if (input_->PushKey(DIK_RIGHT)) {
			moveVelo.x += spd;
			ismoveActive = true;
		}
		if (input_->PushKey(DIK_LEFT)) {
			moveVelo.x -= spd;
			ismoveActive = true;
		}
	}

	if (ismoveActive) {
		Matrix4x4 C_Affine = camera_->GetCameraDirectionToFace();
		moveVelo = TransformNormal(moveVelo, C_Affine);
		moveVelo.y = 0;

		//プレイヤー回転
		Vector2 newR = { moveVelo.x,moveVelo.z };
		playerW_.rotate_.y = CheckR_F_Y(newR);

	}
	playerW_.translate_ = Add(playerW_.translate_, moveVelo);

	

#pragma endregion

	//ダッシュ処理
	if (input_->TriggerKey(DIK_LSHIFT)) {
		stateRequest_ = PlayerState::kDash;
	}

	if (input_->TriggerKey(DIK_X)) {
		stateRequest_ = PlayerState::kATK;
	}


}

void Player::DashUpdate() {

	playerW_.translate_ = Add(playerW_.translate_, dashVelo);

	dashVelo = Subtract(dashVelo, DecelerationVector);

	if (count_++ >= dashTime) {
		stateRequest_ = PlayerState::kStay;
	}


}

void Player::ATKUpdate() {

	animationT_ += 1.0f / (MovingSecond_ * 60);
	Vector3 newR = Esing(weaponStR, weaponEndR, animationT_);
	
	weaponW_.rotate_ = newR;

	weaponW_.UpdateMatrix();

	if (animationT_ >= 1.0f) {
		stateRequest_ = PlayerState::kStay;
	}
	
}
