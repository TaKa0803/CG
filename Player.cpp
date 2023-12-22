#include "Player.h"
#include"TextureManager.h"
#include<imgui.h>
#include"GlobalVariables.h"


const std::array<Player::ConstATK, Player::maxComboNum_>Player::kConstATKs_ = {
	{
		//振りかぶり、攻撃前硬直、攻撃振り時間、硬直（frame)各フェーズ移動速さ
		{0,0,20,0,0.0f,0.0f,0.15f},
		{5,5,15,0,2.0f,0.0f,0.0f},
		{0,0,15,30,0.2f,0.0f,0.0f},

	}
};

Player::~Player() {
	delete collider_;
	delete model_;
	delete weaponM_;
}

void Player::Initialize() {
	input_ = Input::GetInstance();
	
	collider_ = new BoxColider();
	collider_->Initialize(&weaponW_);
	collider_->SetTranslate({ 0,8,0 });
	collider_->SetScale({ 1,6,1 });

	model_ = Model::CreateFromOBJ("ALPlayer");
	model_->IsEnableShader(false);
	world_.UpdateMatrix();
	playertexture = TextureManager::LoadTex("resources/player.png");


	weaponM_ = Model::CreateFromOBJ("weapon/weapon");
	weaponTex_ = TextureManager::LoadTex("resources/weapon/weapon.png");

	weaponW_.SetParent(&world_);


#pragma region globalVariables
	GlobalVariables* globalV = GlobalVariables::GetInstance();

	globalV->CreateGroup(name);


	globalV->AddItem(name, keys[0], cameraDelaySecond_);
	globalV->AddItem(name, keys[1], dashPower_);
	globalV->AddItem(name, keys[2], dashtimeSecond_);
	
	cameraDelaySecond_ = globalV->GetFloatvalue(name, keys[0]);
	dashPower_ = globalV->GetFloatvalue(name, keys[1]);
	dashtimeSecond_ = globalV->GetFloatvalue(name, keys[2]);
	
#pragma endregion

}

void Player::Update() {
#pragma region プレイヤー挙動
	GlobalVariables* globalV = GlobalVariables::GetInstance();
	cameraDelaySecond_ = globalV->GetFloatvalue(name, keys[0]);
	dashPower_ = globalV->GetFloatvalue(name, keys[1]);
	dashtimeSecond_ = globalV->GetFloatvalue(name, keys[2]);
	
#ifdef _DEBUG
	if (ImGui::BeginMenu("player")) {
		ImGui::DragFloat3("pos", &world_.translate_.x, 0.01f);
		ImGui::DragFloat3("rotate", &world_.rotate_.x, 0.01f);
		ImGui::DragFloat3("scale", &world_.scale_.x, 0.01f);
		ImGui::Text("%d", nowParent);
		ImGui::Text("state : %d", pState_);
		ImGui::Text("combo : %d", workATK_.comboIndex);
		ImGui::EndMenu();
	}
#endif // _DEBUG

	

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

		case Player::PlayerState::kjump:
			JumpInitialize();
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
	case Player::PlayerState::kjump:
		JumpUpdate();
		break;
	default:
		break;
	}


	

	world_.UpdateMatrix();

	collider_->DebugImGui("player colloder");
	collider_->Update();
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
	model_->Draw(world_.matWorld_, camera_->GetViewProjectionMatrix(), playertexture);

	if (pState_ == PlayerState::kATK) {
		weaponM_->Draw(weaponW_.matWorld_, camera_->GetViewProjectionMatrix(), weaponTex_);
	}

	//collider_->Draw(camera_->GetViewProjectionMatrix());
}

bool Player::SetHitEnemy(int enemy) {
	
	//すでに当たっていたらfalse
	for (int ene : hitenemies_) {
		if (ene == enemy) {
			return false;
		}
	}

	//当たっていなかったのでいれてtrue
	hitenemies_.push_back(enemy);
	return true;

}

void Player::OnCollision(int hitparent, const WorldTransform* parent) {

	if (pState_ != PlayerState::kStay && pState_ != PlayerState::kATK) {
		stateRequest_ = PlayerState::kStay;
	}

	//if (InCollision(p2, pParentP)) {
			//ペアレントが違うものだったら
	if (nowParent != hitparent) {
		nowParent = hitparent;
		//台座から見たプレイヤーの座標取得
		Vector3 pos = GetmatT() - parent->GetMatWorldTranslate();


		world_.translate_ = pos;
		world_.translate_.y = 2.0f;
		world_.SetParent(parent);


		world_.UpdateMatrix();
	}

}

void Player::NoCollision() {
	//親子関係を消して処理
	world_.translate_ = GetmatT();
	world_.SetParent();
	world_.UpdateMatrix();

	pState_ = PlayerState::kjump;
	nowParent = 0;
}

void Player::SetStartPosition() {
	world_.translate_ = startPos;
	world_.SetParent();
	stateRequest_ = PlayerState::kStay;
	nowParent = 0;
}

bool Player::FallingCheck() {
	//落ちたら初期位置へ
	if (world_.translate_.y <= -50) {
		SetStartPosition();
		return true;
	}
	return false;
}

void Player::StayInitialize() {
	velo_ = { 0.0f,0.0f,0.0f };
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
			moveVelo.SetNormalize();
			moveVelo = dashPower_ * moveVelo;

			Matrix4x4 C_Affine = camera_->GetMainCamera().matWorld_;
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
			moveVelo *= dashPower_;
		}
		Matrix4x4 C_Affine = camera_->GetMainCamera().matWorld_;
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

	hitenemies_.clear();

	workATK_ = { 0,0,0,false };

	if (lockOn_->GetTarget() != nullptr) {

		Vector3 epos = lockOn_->GetTarget()->GetWorld().GetMatWorldTranslate();

		Vector3 pPos = world_.GetMatWorldTranslate();

		Vector3 velo = epos - pPos;

		float yrotate = GetYRotate(Vector2(velo.x, velo.z));

		world_.rotate_.y = yrotate;


	}
}

void Player::JumpInitialize() {
	//初速度
	velo_ = { 0.0f,jumpPower_,0.0f };
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
			moveVelo *= spd;

			Matrix4x4 C_Affine = camera_->GetMainCamera().matWorld_;
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
		Matrix4x4 C_Affine = camera_->GetMainCamera().matWorld_;
		moveVelo = TransformNormal(moveVelo, C_Affine);
		moveVelo.y = 0;

		Vector2 newR = { moveVelo.x,moveVelo.z };
		world_.rotate_.y = GetYRotate(newR);
	}

	moveVelo.y += gravity;

	world_.translate_ += moveVelo;

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
			moveVelo *= spd;

			/*
			Matrix4x4 C_Affine = camera_->GetCameraDirectionToFace();
			moveVelo = TransformNormal(moveVelo,C_Affine);
			moveVelo.y = 0;
			*/
			ismoveActive = true;
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

	//プレイヤー傾き処理
	if (ismoveActive) {
		Matrix4x4 C_Affine = camera_->GetMainCamera().matWorld_;
		moveVelo = TransformNormal(moveVelo, C_Affine);
		moveVelo.y = 0;

		//プレイヤー回転
		Vector2 newR = { moveVelo.x,moveVelo.z };

		

		world_.rotate_.y = GetYRotate(newR);



	}



	world_.translate_ += moveVelo;



#pragma endregion

	//ダッシュ処理
	if (/*input_->TriggerKey(DIK_LSHIFT)||*/input_->IsTriggerButton(kButtonX)) {
		stateRequest_ = PlayerState::kDash;
	}

	if (input_->TriggerKey(DIK_V) || input_->IsTriggerButton(kButtonB)) {
		stateRequest_ = PlayerState::kATK;
	}
	if (/*input_->TriggerKey(DIK_SPACE) || */input_->IsTriggerButton(kButtonA)) {
		stateRequest_ = PlayerState::kjump;
	}


}

void Player::DashUpdate() {

	world_.translate_ += dashVelo;

	dashVelo -= DecelerationVector;

	if (count_++ >= dashTime) {
		stateRequest_ = PlayerState::kStay;
	}


}

void Player::NextATK() {
	if (workATK_.comboNext) {
		workATK_.comboNext = false;
		workATK_.comboIndex++;

		workATK_.attackParameter = 0;
		workATK_.inComboPhase = 0;
		hitenemies_.clear();
	}
	else {
		workATK_.comboIndex = maxComboNum_;
	}
}

void Player::ATKUpdate() {

	//ターゲットに攻撃向ける処理
	if (lockOn_->GetTarget() != nullptr) {

		Vector3 epos = lockOn_->GetTarget()->GetWorld().GetMatWorldTranslate();

		Vector3 pPos = world_.GetMatWorldTranslate();

		Vector3 velo = epos - pPos;

		float yrotate = GetYRotate(Vector2(velo.x, velo.z));

		world_.rotate_.y = yrotate;
	}


	//コンボ数が最大になるまで処理
	if (workATK_.comboIndex != maxComboNum_) {

		//攻撃ボタントリガーで次へのフラグON
		if (input_->IsTriggerButton(kButtonB)) {
			workATK_.comboNext = true;
		}

		


#pragma region モデルアニメーション
		float T=0;
		Vector3 newR;

		//パーツのアニメーション
		//各種状態による処理
		switch (workATK_.inComboPhase) {
		case 0:
			if (kConstATKs_[workATK_.comboIndex].anicipationTime != 0) {
				T = (float)workATK_.attackParameter / (float)kConstATKs_[workATK_.comboIndex].anicipationTime;
			}
			if (workATK_.comboIndex != 0) {
				newR = Esing(weaponEndR[workATK_.comboIndex-1], weaponStR[workATK_.comboIndex], T);

				weaponW_.rotate_ = newR;
			}
			break;

		case 1:

			break;

		case 2:
			if (kConstATKs_[workATK_.comboIndex].swingTime != 0) {
				T = (float)workATK_.attackParameter / (float)kConstATKs_[workATK_.comboIndex].swingTime;
			}
			newR = Esing(weaponStR[workATK_.comboIndex], weaponEndR[workATK_.comboIndex], T);

			weaponW_.rotate_ = newR;


			break;

		case 3:

			break;

		}
#pragma endregion

#pragma region 状態管理処理
		//各種状態による処理
		switch (workATK_.inComboPhase) {
		case 0:
			if (++workATK_.attackParameter >= kConstATKs_[workATK_.comboIndex].anicipationTime) {
				workATK_.inComboPhase++;
				workATK_.attackParameter = 0;
				
			}
			break;

		case 1:
			if (++workATK_.attackParameter >= kConstATKs_[workATK_.comboIndex].chargeTime) {
				workATK_.inComboPhase++;
				workATK_.attackParameter = 0;
				
			}
			break;

		case 2:
			if (++workATK_.attackParameter >= kConstATKs_[workATK_.comboIndex].swingTime) {
				workATK_.inComboPhase++;
				workATK_.attackParameter = 0;
				
			}
			break;

		case 3:
			if (++workATK_.attackParameter >= kConstATKs_[workATK_.comboIndex].recoveryTime) {
				NextATK();
			}
			break;

		}

	}
	else {
		//コンボ外で終了

		stateRequest_ = PlayerState::kStay;
	}


#pragma endregion

		
	



	weaponW_.UpdateMatrix();
}

void Player::JumpUpdate() {

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
			moveVelo *= spd;

			Matrix4x4 C_Affine = camera_->GetMainCamera().matWorld_;
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
		Matrix4x4 C_Affine = camera_->GetMainCamera().matWorld_;
		moveVelo = TransformNormal(moveVelo, C_Affine);
		moveVelo.y = 0;

		//プレイヤー回転
		Vector2 newR = { moveVelo.x,moveVelo.z };
		world_.rotate_.y = GetYRotate(newR);

	}
	world_.translate_ += moveVelo;



#pragma endregion


	world_.translate_ += velo_;

	velo_.y -= jumpPower_ / (60.0f * zeroSecond);

}


