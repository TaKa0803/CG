#include "Player.h"
#include"TextureManager.h"
#include<imgui.h>


Player::~Player() {
	delete playerM_;
}

void Player::Initialize() {
	input_ = Input::GetInstance();

	playerM_ = Model::CreateFromOBJ("ALPlayer");
	playerM_->IsEnableShader(false);
	playerW_.UpdateMatrix();
	playertexture = TextureManager::LoadTex("resources/player.png");


}

void Player::Update() {
#pragma region プレイヤー挙動
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

void Player::Draw() {
	playerM_->Draw(playerW_.matWorld_, camera_->GetViewProjectionMatrix(), playertexture);

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
}

void Player::ATKInitialize() {
}

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
	stateRequest_ = PlayerState::kStay;
}
