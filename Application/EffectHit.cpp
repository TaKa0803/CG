#include "EffectHit.h"
#include"TextureManager/TextureManager.h"
#include"imgui.h"

EffectHit* EffectHit::GetInstance() {
	static EffectHit instance;
	return&instance;
}

void EffectHit::Initialize(const Camera* camera) {

	camera_ = camera;

	texture_ = TextureManager::LoadTex("resources/awa/awa.png");

	for (int i = 0; i < num; i++) {
		Model* newmo = Model::CreateFromOBJ("awa/awa");

		Vector3 velo = {
			(rand() % 10 - 5) / 10.0f,
			(rand() % 10) / 10.0f,
			(rand() % 10 - 5) / 10.0f,
		};
		WorldTransform world;

		deadTime dT;
		dT = {
			world,
			velo,
			(rand() % 5 + 15) ,
			0,
			true
		};

		std::pair<Model*, deadTime>newpai = std::make_pair(newmo, dT);

		effects_.emplace_back(newpai);

	}
}

void EffectHit::Update() {
	for (std::pair<Model*, deadTime>& effect : effects_) {
		if (!effect.second.isdead_) {
			//削除処理
			if (effect.second.time_++ >= effect.second.maxTime_) {
				effect.second.isdead_ = true;
				break;
			}

			ImGui::Begin("effe");
			ImGui::DragFloat3("rotate", &effect.second.world_.rotate_.x, 0.01f);
			ImGui::End();

#pragma region こちらを向く処理
			//カメラ
			Vector3 c_pos = camera_->GetMainCamera().GetMatWorldTranslate();
			//対象物
			Vector3 e_pos = effect.second.world_.GetMatWorldTranslate();

			//対象物からカメラへの方向
			Vector3 veloE2C = c_pos - e_pos;

			//正規化
			
			float xzleng = sqrtf(veloE2C.x * veloE2C.x + veloE2C.y * veloE2C.y);

			float RX = GetYRotate(Vector2(xzleng, veloE2C.y));
			
			effect.second.world_.rotate_.x = RX;

			float RY = GetYRotate(Vector2(veloE2C.x, veloE2C.z));
			effect.second.world_.rotate_.y = RY;

#pragma endregion


			//透明度処理
			float t = (float)effect.second.time_ / (float)effect.second.maxTime_;

			float alpha = 1 - t;

			effect.first->SetAlpha(alpha);

			




			effect.second.world_.translate_ += effect.second.velo_;
			//行列更新
			effect.second.world_.UpdateMatrix();
		}
	}
}

void EffectHit::Draw() {
	for (std::pair<Model*, deadTime>& effect : effects_) {
		if (!effect.second.isdead_) {
			effect.first->Draw(effect.second.world_.matWorld_, camera_->GetViewProjectionMatrix(), texture_);
		}
	}
}

void EffectHit::Finalize() {


	for (std::pair<Model*, deadTime>&ef : effects_) {
		delete ef.first;
		ef.first = nullptr;
	}
}

void EffectHit::SpawnE(const WorldTransform& world, int setnum) {

	int anum = 0;

	for (int i = 0; i < num; i++) {
		if (effects_[i].second.isdead_) {
			effects_[i].second.isdead_ = false;
			effects_[i].second.world_ = world;
			effects_[i].second.world_.translate_ = world.GetMatWorldTranslate();
			effects_[i].second.world_.SetParent();
			effects_[i].second.world_.scale_ = { 0.5f,0.5f,0.5f };
			effects_[i].second.time_ = 0;
			anum++;

			if (anum == setnum) {
				break;
			}
		}
	}
	
}
