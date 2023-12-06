#include "DebugScene.h"
#include"TextureManager.h"
#include"imgui.h"

#include"DirectXFunc.h"
#include"function.h"

#include"RandomNum.h"

#include"Input.h"

Particle MakeNewParticle(const Vector3& spawnPos, const Vector3& emiterSize, const Vector3& maxVelo, const Vector3& minVelo,const Vector4& colorMin, const Vector4& colorMax,const Vector2&lifeTimeminXmaxY) {
	Particle ans;

	ans.position = spawnPos + RandomNumber::Get(-emiterSize / 2,emiterSize / 2);
	

	ans.velocity = RandomNumber::Get(minVelo, maxVelo);

	ans.color = RandomNumber::Get(colorMin, colorMax);

	ans.lifeTime = RandomNumber::Get(lifeTimeminXmaxY.x, lifeTimeminXmaxY.y);
	ans.currentTime = 0;

	return ans;
}



void DebugScene::Initialize() {
	model_ = Model::CreateFromOBJ("fence/fence");
	//texture = TextureManager::LoadTex("resources/fence/fence.png");

	//model_ = Model::CreateFromOBJ("ALPlayer");
	texture = TextureManager::LoadTex("resources/uvChecker.png");

	camera_.Initialize();
	camera_.SetTarget(&world_);

	sprite_ = new Sprite();
	//sprite_ = Sprite::CreateInstancing(texture, { 128,128 },kNuminstancing);
	sprite_ = Sprite::CreateInstancing(texture, { 256,256 },kNuminstancing+5);
	
	for (uint32_t index = 0; index < (uint32_t)kNuminstancing; ++index) {
		insPos[index] = MakeNewParticle(center, { 0,0,0 }, minvelo, velo,colormin,color,{60,180});
	}

	
}

void DebugScene::Update() {
	world_.DrawDebug("box");
	camera_.DrawDebugWindow("camera");

	model_->DebugParameter("box");
	sprite_->DrawDebugImGui("sprite");


	Input* input = Input::GetInstance();

	if (input->TriggerKey(DIK_SPACE)) {
		for (uint32_t index = 0; index < (uint32_t)kNuminstancing; ++index) {
			insPos[index] = MakeNewParticle(center, { 0,0,0 }, minvelo, velo, colormin, color, { 60,180 });
		}
		checkUpdate_ = true;
	}


	ImGui::Begin("InGame", nullptr, ImGuiWindowFlags_MenuBar);
	ImGui::BeginMenuBar();

	ImGui::Checkbox("isMove", &checkUpdate_);


	for (uint32_t index = 0; index < (uint32_t)kNuminstancing; ++index) {

		if (insPos[index].currentTime++ <= insPos[index].lifeTime) {


			if (checkUpdate_) {
				

				insPos[index].position += insPos[index].velocity * kDeltaTime;
				float alpha = 1.0f - (insPos[index].currentTime / insPos[index].lifeTime);

				if (alpha <= 0) {
					alpha = 0;
				}

				insPos[index].color.w = alpha;
			}

			if (ImGui::BeginMenu("aho")) {
				ImGui::ColorEdit4("set color", &insPos[index].color.x);
				ImGui::EndMenu();
			}

			sprite_->SetParticle(&insPos[index]);
		}
	}

	world_.UpdateMatrix();
	camera_.Update();

	ImGui::EndMenuBar();
	ImGui::End();
}

void DebugScene::Draw() {
	model_->Draw(world_.matWorld_, camera_.GetViewProjectionMatrix(), texture);

	sprite_->DrawInstancing();
	//sprite_->Draw();
}

void DebugScene::Finalize() {
	delete model_;
	delete sprite_;
}
