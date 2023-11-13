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


	eh = TextureManager::LoadTex("resources/head.png");
	eWeapon = TextureManager::LoadTex("resources/Eweapon.png");

	ehead_ = Model::CreateFromOBJ("head");
	eLA_ = Model::CreateFromOBJ("Eweapon");
	eRA_ = Model::CreateFromOBJ("Eweapon");

	ehead_->IsEnableShader(false);
	eLA_->IsEnableShader(false);
	eRA_->IsEnableShader(false);

	goalTex = TextureManager::LoadTex("resources/goal.png");
	goal_ = Model::CreateFromOBJ("goal");
	goal_->IsEnableShader(false);
	goalT_.SetParent(&planeTrans3_);


	goalT_.translate_.y = 2;

	eWorld_.translate_.y = 2;

	eWorld_.SetParent(&planeTrans2_);
	ehT_.SetParent(&eWorld_);
	eLT_.SetParent(&ehT_);
	eRT_.SetParent(&ehT_);

	eLT_.translate_ = { -2.0f, 0, 0 };
	eRT_.translate_ = { 2.0f, 0, 0 };
	eRT_.rotate_.x = 3.14f / 2.0f;

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

	player_->Update();

	camera.DrawDebugWindow("Camera");
	camera.Update();

#pragma region 天球
	if (ImGui::BeginMenu("skydome")) {
		ImGui::DragInt("scale", &scaleNum);
		ImGui::EndMenu();
	}
	skydomeTrans_.scale_ = { (float)scaleNum,(float)scaleNum,(float)scaleNum };
	skydomeTrans_.UpdateMatrix();
#pragma endregion
#pragma region 敵

	if (ImGui::BeginMenu("enemy")) {
		ImGui::DragFloat3("pos", &eWorld_.translate_.x, 0.01f);
		ImGui::EndMenu();
	}
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

	ehead_->Draw(ehT_.matWorld_, camera.GetViewProjectionMatrix(), eh);
	eLA_->Draw(eLT_.matWorld_, camera.GetViewProjectionMatrix(), eWeapon);
	eRA_->Draw(eRT_.matWorld_, camera.GetViewProjectionMatrix(), eWeapon);


	goal_->Draw(goalT_.matWorld_, camera.GetViewProjectionMatrix(), goalTex);

}
void InGame::Finalize() {
	
	delete player_;

	delete plane1;
	delete plane2;
	delete plane3;

	delete skydome_;

	delete ehead_;
	delete eLA_;
	delete eRA_;

	delete goal_;
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
		

		player_->OnCollision(1,&planeTrans1_);
		
		
	}
	else if (InCollision(pAABB, p2)) {
		ishit = true;
		player_->OnCollision(1, &planeTrans2_);

		
		
	}
	else if (InCollision(pAABB, p3)) {
		ishit = true;
		player_->OnCollision(1, &planeTrans3_);		
	}
	if (ishit == false) {
		player_->NoCollision();
	}


	if (InCollision(pAABB, goa) || InCollision(pAABB, ene)) {
		player_->SetStartPosition();
	}

}
