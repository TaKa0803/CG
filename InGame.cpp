#include"InGame.h"
#include"TextureManager.h"
#include"externals/Imgui/imgui.h"

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
}

void InGame::Update() {
#pragma region 全部の更新処理とCBuffer転送


#pragma region プレイヤー挙動

#pragma endregion

	ImGui::Begin("obj");
	ImGui::DragFloat3("pos", &playerW_.translate_.x, 0.01f);
	ImGui::DragFloat3("rotate", &playerW_.rotate_.x, 0.01f);
	ImGui::End();

	Vector3 moveVelo = { 0,0,0 };

	if (input->PushKey(DIK_UP)) {
		moveVelo.z += 1;
	}
	if (input->PushKey(DIK_DOWN)) {
		moveVelo.z -= 1;
	}
	if (input->PushKey(DIK_RIGHT)) {
		moveVelo.x += 1;
	}
	if (input->PushKey(DIK_LEFT)) {
		moveVelo.x -= 1;
	}

	playerW_.translate_ = Add(playerW_.translate_, moveVelo);
	playerW_.UpdateMatrix();
	
#pragma endregion

#pragma region 台座処理
	ImGui::Begin("plane");
	ImGui::DragFloat3("pos1", &planeTrans1_.translate_.x, 0.01f);
	ImGui::DragFloat3("pos2", &planeTrans2_.translate_.x, 0.01f);
	ImGui::DragFloat3("pos3", &planeTrans3_.translate_.x, 0.01f);
	ImGui::End();

	planeTrans1_.UpdateMatrix();
	planeTrans2_.UpdateMatrix();
	planeTrans3_.UpdateMatrix();

#pragma endregion


#pragma region カメラ処理
	ImGui::Begin("Camera");
	ImGui::DragFloat3("Camera translate", &parentCmaera.translate_.x, 0.01f);
	ImGui::DragFloat3("Camera rotate", &parentCmaera.rotate_.x, 0.01f);
	ImGui::End();

	//カメラをプレイヤーの座標に合わせる
	parentCmaera.translate_ = playerW_.translate_;

	parentCmaera.UpdateMatrix();
	
	//カメラ処理
	cameraTransform.UpdateMatrix();


	//親の行列掛け算
	Matrix4x4 viewMatrix = Inverse(cameraTransform.matWorld_);
	//透視投影行列
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
	VP = Multiply(viewMatrix, projectionMatrix);


#pragma endregion

}

void InGame::Draw() {
	
	playerM_->Draw(playerW_.matWorld_, VP, playertexture);


	plane1->Draw(planeTrans1_.matWorld_, VP, planeTex_);
	plane2->Draw(planeTrans2_.matWorld_, VP, planeTex_);
	plane3->Draw(planeTrans3_.matWorld_, VP, planeTex_);

}
