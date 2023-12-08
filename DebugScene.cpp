#include "DebugScene.h"
#include"TextureManager.h"
#include"imgui.h"

#include"DirectXFunc.h"
#include"function.h"

#include"RandomNum.h"

#include"Input.h"





void DebugScene::Initialize() {
	model_ = Model::CreateFromOBJ("fence/fence");
	//texture = TextureManager::LoadTex("resources/fence/fence.png");

	//model_ = Model::CreateFromOBJ("ALPlayer");
	//texture = TextureManager::LoadTex("resources/circle.png");
	texture = TextureManager::uvChecker_;

	camera_.Initialize();
	camera_.SetTarget(&world_);

	//sprite_ = new Sprite();
	//sprite_ = Sprite::CreateInstancing(texture, { 256,256 },kNuminstancing+5);
	
	for (uint32_t index = 0; index < (uint32_t)kNuminstancing; ++index) {
		insPos[index] = MakeNewParticle(center, { 0,0,0 }, velo,color,{60,180});
	}

	pE_ = new ParticleEmiter();
	pE_ = ParticleEmiter::Create3D(&camera_,texture, kNuminstancing, { 512,512 }, { 512,512 });
	
}

void DebugScene::Update() {
	world_.DrawDebug("box");
	camera_.DrawDebugWindow("camera");

	//model_->DebugParameter("box");
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


	float scale = insPos[0].world_.scale_.x;
	if (ImGui::BeginMenu("aho")) {
		ImGui::DragFloat("scale", &scale);
		ImGui::EndMenu();
	}
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

		
		insPos[index].world_.scale_ = {scale,scale,scale};
		//sprite_->SetParticle(&insPos[index]);
		pE_->SetParticle(&insPos[index]);
	}

	pE_->DebugImGui("effects");

	world_.UpdateMatrix();
	camera_.Update();

	ImGui::EndMenuBar();
	ImGui::End();
}

void DebugScene::Draw() {
	//model_->Draw(world_.matWorld_, camera_.GetViewProjectionMatrix(), texture);

	//sprite_->DrawInstancing();

	pE_->Draw3D();
	//sprite_->Draw();
}

void DebugScene::Finalize() {
	delete model_;
	delete sprite_;
	delete pE_;
}
