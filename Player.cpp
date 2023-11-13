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

	if (pState_ == kFalling) {
		moveVelo.y += gravity;
	}


	playerW_.translate_ = Add(playerW_.translate_, moveVelo);

	if (playerW_.translate_.y <= -50) {
		playerW_.translate_ = startPos;
		playerW_.SetParent(nullptr);
		pState_ = kStay;
		nowParent = 0;
	}

	playerW_.UpdateMatrix();
#pragma endregion
}

void Player::Draw() {
	playerM_->Draw(playerW_.matWorld_, camera_->GetViewProjectionMatrix(), playertexture);

}

void Player::OnCollision(int hitparent, const WorldTransform* parent) {
	pState_ = kStay;

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
	pState_ = kFalling;
	nowParent = 0;
}

void Player::SetStartPosition() {
	playerW_.translate_ = startPos;
	playerW_.SetParent();
	pState_ = kStay;
	nowParent = 0;
}
