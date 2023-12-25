#include "DebugScene.h"
#include"TextureManager/TextureManager.h"
#include<imgui.h>

#include"DirectXFunc/DirectXFunc.h"
#include"functions/function.h"

#include"RandomNum/RandomNum.h"

#include"Input/Input.h"




void DebugScene::Initialize() {
	//model_ = Model::CreateFromOBJ("fence/fence");
	//texture = TextureManager::LoadTex("resources/fence/fence.png");

	model_ = Model::CreateFromOBJ("ALPlayer");

	model2_ = Model::CreateFromOBJ("ALPlayer");
	//texture = TextureManager::LoadTex("resources/circle.png");
	//texture = TextureManager::LoadTex("resources/circle.png");
	texture = TextureManager::uvChecker_;

	camera_.Initialize();
	camera_.SetTarget(&world_);

	//sprite_ = new Sprite();
	//sprite_ = Sprite::Create(texture,{512,512},{256,256});
	//sprite_->SetPosition({ 640, 360});
	//sprite_->SetScale({ 512,512,1 });

	for (uint32_t index = 0; index < (uint32_t)kNuminstancing; ++index) {
		insPos[index] = MakeNewParticle(center, { 0,0,0 }, velo,color,{60,180});
	}

	pE_ = new ParticleEmiter();
	pE_ = ParticleEmiter::Create3D(&camera_,texture, kNuminstancing, { 512,512 }, { 256,256 });



#pragma region FBXの処理
	
#pragma endregion

	

}

void DebugScene::Update() {
	world_.DrawDebug("box1");
	world2_.DrawDebug("box2");

	camera_.DrawDebugWindow("camera");

	model_->DebugParameter("box");
	model2_->DebugParameter("box2");
	//sprite_->DrawDebugImGui("sprite");

	

	Input* input = Input::GetInstance();

	if (input->TriggerKey(DIK_SPACE)) {
		for (uint32_t index = 0; index < (uint32_t)kNuminstancing; ++index) {
			insPos[index] = MakeNewParticle(center, { 0,0,0 },  velo, color, { 60,180 });
		}
		checkUpdate_ = true;
	}


	ImGui::Begin("InGame", nullptr, ImGuiWindowFlags_MenuBar);
	ImGui::BeginMenuBar();

	ImGui::Checkbox("isMove", &checkUpdate_);


	////float scale = sprite_->GetScale().x;
	//if (ImGui::BeginMenu("aho")) {

	////	ImGui::DragFloat("scale", &scale);
	//	ImGui::EndMenu();
	//}

	//sprite_->SetScale(scale);

	ImGui::EndMenuBar();
	ImGui::End();

		for (uint32_t index = 0; index < (uint32_t)kNuminstancing; ++index) {
			if (checkUpdate_) {
				if (insPos[index].currentTime++ <= insPos[index].lifeTime) {
					insPos[index].world_.translate_ += insPos[index].velocity * kDeltaTime;
					float alpha = 1.0f - (insPos[index].currentTime / insPos[index].lifeTime);
					if (alpha <= 0) {
						alpha = 0;
					}
					insPos[index].color.w = alpha;
				}
			}


			//insPos[index].world_.scale_ = { scale,scale,scale };
			//sprite_->SetParticle(&insPos[index]);
			pE_->SetParticle(&insPos[index]);
			
			ImGui::Begin("R");
			ImGui::DragFloat3("rotate", &insPos[index].world_.rotate_.x, 0.01f);
			ImGui::End();
		}

	
	pE_->DebugImGui("effects");
	pE_->Update();

	world_.UpdateMatrix();
	world2_.UpdateMatrix();
	camera_.Update();

	
}

void DebugScene::Draw() {
	model_->Draw(world_.matWorld_, camera_.GetViewProjectionMatrix());

	model2_->Draw(world2_.matWorld_, camera_.GetViewProjectionMatrix(), texture);
	//sprite_->DrawInstancing();

	pE_->Draw3D();
	//sprite_->Draw();
}

void DebugScene::Finalize() {
	delete model_;
	delete model2_;
	delete sprite_;
	delete pE_;
}
