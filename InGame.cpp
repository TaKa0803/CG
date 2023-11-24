#include"InGame.h"
#include"TextureManager.h"
#include"externals/Imgui/imgui.h"
#include<cmath>
#include<iostream>


//AABB同士
bool InCollision(const AABB& a, const AABB& b) {

	if ((a.minV.x <= b.maxV.x && a.maxV.x >= b.minV.x) &&//x軸
		(a.minV.y <= b.maxV.y && a.maxV.y >= b.minV.y) &&
		(a.minV.z <= b.maxV.z && a.maxV.z >= b.minV.z)
		) {
		return true;
	}
	return false;
}



void InGame::Initialize() {

	player_ = new Player();
	player_->Initialize();

	camera.Initialize();
	camera.SetTarget(&player_->GetWorld());

	player_->SetCamera(camera);


	plane1 = Model::CreateFromOBJ("Plane");
	plane2 = Model::CreateFromOBJ("Plane");
	plane3 = Model::CreateFromOBJ("Plane");

	planeTex_ = TextureManager::LoadTex("resources/Plane.png");

	skydome_ = Model::CreateFromOBJ("sssssk");
	//skydome_->IsEnableShader(false);
	skydomeTex = TextureManager::LoadTex("resources/Space.png");




	planeTrans1_.translate_ = { 0.0f,-2.0f,0.0f };
	planeTrans2_.translate_ = { 0.0f,-2.0f,planeSize * 2.0f };
	planeTrans3_.translate_ = { 0.0f,-2.0f,planeSize * 4.0f };



	goalTex = TextureManager::LoadTex("resources/goal.png");
	goal_ = Model::CreateSphere(128 ,true);
	goalT_.SetParent(&planeTrans3_);


	goalT_.translate_.y = 2;

	WorldTransform world;
	world.translate_.x = -2;

	enemy_ =new Enemy();
	enemy_->Initialize(world);
	enemy_->SetParent(&planeTrans2_);


	lockOn_ = new LockOn();
	lockOn_->Initialize();
}

void InGame::Update() {
#ifdef _DEBUG
	ImGui::Begin("InGame", nullptr, ImGuiWindowFlags_MenuBar);
	ImGui::BeginMenuBar();
#endif // _DEBUG


#pragma region 台座処理
#ifdef _DEBUG
	if (ImGui::BeginMenu("plane")) {
		ImGui::DragFloat3("pos1", &planeTrans1_.translate_.x, 0.01f);
		ImGui::DragFloat3("pos2", &planeTrans2_.translate_.x, 0.01f);
		ImGui::DragFloat3("pos3", &planeTrans3_.translate_.x, 0.01f);

		ImGui::DragFloat3("posr1", &planeTrans1_.rotate_.x, 0.01f);
		ImGui::DragFloat3("posr2", &planeTrans2_.rotate_.x, 0.01f);
		ImGui::DragFloat3("posr3", &planeTrans3_.rotate_.x, 0.01f);
		ImGui::EndMenu();
	}
#endif // _DEBUG





	PlaneUpdate();

	planeTrans1_.UpdateMatrix();
	planeTrans2_.UpdateMatrix();
	planeTrans3_.UpdateMatrix();

#pragma endregion

	enemy_->Update();

	player_->Update();

	camera.DrawDebugWindow("Camera");
	camera.Update();

	player_->DashCameraUpdate(camera);

#pragma region 天球
	if (ImGui::BeginMenu("skydome")) {
		ImGui::DragInt("scale", &scaleNum);
		ImGui::EndMenu();
	}
	skydomeTrans_.scale_ = { (float)scaleNum,(float)scaleNum,(float)scaleNum };
	skydomeTrans_.UpdateMatrix();
#pragma endregion

#pragma region ゴール
	goalT_.UpdateMatrix();
#pragma endregion


	lockOn_->Update();

	Collision();

#ifdef _DEBUG
	ImGui::EndMenuBar();
	ImGui::End();
#endif // _DEBUG
}

void InGame::Draw() {

	player_->Draw();

	plane1->Draw(planeTrans1_.matWorld_, camera.GetViewProjectionMatrix(), planeTex_);
	plane2->Draw(planeTrans2_.matWorld_, camera.GetViewProjectionMatrix(), planeTex_);
	plane3->Draw(planeTrans3_.matWorld_, camera.GetViewProjectionMatrix(), planeTex_);

	skydome_->Draw(skydomeTrans_.matWorld_, camera.GetViewProjectionMatrix(), skydomeTex);

	enemy_->Draw(camera.GetViewProjectionMatrix());

	goal_->Draw(goalT_.matWorld_, camera.GetViewProjectionMatrix(), goalTex);

	lockOn_->Draw();
}
void InGame::Finalize() {

	delete player_;

	delete enemy_;

	delete plane1;
	delete plane2;
	delete plane3;

	delete skydome_;

	
	delete goal_;

	delete lockOn_;
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
	AABB pAABB = player_->GetAABB();



	AABB p1 = {
		.minV{GetmatPW1().x - planeSize,GetmatPW1().y - 1.0f,GetmatPW1().z - planeSize},
		.maxV{GetmatPW1().x + planeSize,GetmatPW1().y + 1.0f,GetmatPW1().z + planeSize}
	};

	AABB p2 = {
		.minV{GetmatPW2().x - planeSize,GetmatPW2().y - 1.0f,GetmatPW2().z - planeSize},
		.maxV{GetmatPW2().x + planeSize,GetmatPW2().y + 1.0f,GetmatPW2().z + planeSize}
	};
	AABB p3 = {
		.minV{GetmatPW3().x - planeSize,GetmatPW3().y - 1.0f,GetmatPW3().z - planeSize},
		.maxV{GetmatPW3().x + planeSize,GetmatPW3().y + 1.0f,GetmatPW3().z + planeSize}
	};

	AABB goa = {
		.minV{GetgoalMat().x - gSize_,GetgoalMat().y - gSize_,GetgoalMat().z - gSize_},
		.maxV{GetgoalMat().x + gSize_,GetgoalMat().y + gSize_,GetgoalMat().z + gSize_}
	};

	AABB ene = {
		.minV{enemy_->GetWorld().GetMatWorldTranslate().x - enemy_->GetSize(),enemy_->GetWorld().GetMatWorldTranslate().y - enemy_->GetSize(),enemy_->GetWorld().GetMatWorldTranslate().z - enemy_->GetSize()},
		.maxV{enemy_->GetWorld().GetMatWorldTranslate().x + enemy_->GetSize(),enemy_->GetWorld().GetMatWorldTranslate().y + enemy_->GetSize(),enemy_->GetWorld().GetMatWorldTranslate().z + enemy_->GetSize()}
	};

	bool ishit = false;


	//土台1とのコリジョン
	if (InCollision(pAABB, p1)) {
		ishit = true;
		player_->OnCollision(1, &planeTrans1_);
	}
	else if (InCollision(pAABB, p2)) {
		ishit = true;
		player_->OnCollision(2, &planeTrans2_);
	}
	else if (InCollision(pAABB, p3)) {
		ishit = true;
		player_->OnCollision(3, &planeTrans3_);
	}
	if (ishit == false) {
		player_->NoCollision();
	}

	//ゴール
	if (InCollision(pAABB, goa)) {
		player_->SetStartPosition();
		enemy_->SetDead(false);
	}

	//敵
	if (InCollision(pAABB, ene) && !enemy_->GetDead()) {
		player_->SetStartPosition();
		
		enemy_->SetDead(false);
	}


	//武器とのコリジョン
	if (player_->IsStateATK()) {
		Vector3 weaponPos = player_->GetWeaponWorld().GetMatWorldTranslate();
		Vector3 ePos = enemy_->GetWorld().GetMatWorldTranslate();

		float hlong = Length(weaponPos- ePos);

		if (hlong <= player_->GetWeaponRadius() + enemy_->GetSize()) {
			enemy_->SetDead(true);
		}
	}

}