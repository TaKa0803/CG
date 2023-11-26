#include "LockOn.h"
#include"TextureManager.h"

LockOn::~LockOn() {
	delete lockOn_;
}

void LockOn::Initialize() {
	int texture = TextureManager::LoadTex("resources/lockOn.png");
	lockOn_ = Sprite::Create(texture,{64,64});
}


void LockOn::Update(Camera*camera) {

	//マークの処理
	if (target_) {

		Vector3 pos = target_->GetWorld().GetMatWorldTranslate();

		Matrix4x4 matviewport = MakeViewPortMatrix(0, 0, (float)WinApp::kClientWidth, (float)WinApp::kClientHeight, 0, 1);

		Matrix4x4 matVPV = camera->GetViewProjectionMatrix() * matviewport;

		pos = Transform(pos, matVPV);

		//V2に変換
		Vector2 newp = { pos.x,pos.y };
		lockOn_->SetPosition(newp);


#pragma region カメラ処理
		//座標取得
		Vector3 tpos = target_->GetWorld().GetMatWorldTranslate();

		//ベース位置取得
		Vector3 Basepos = base_->GetMatWorldTranslate();

		Vector3 offset = tpos - Basepos;
		offset = Normalize(offset);
		//回転量計算
		float yrotate = CheckR_F_Y(Vector2(offset.x, offset.z));

		camera->SetCameraR_Y(yrotate);
#pragma endregion


		
#pragma region キャンセル処理
		//ベース位置取得
		Vector3 Bpos = base_->GetMatWorldTranslate();

		//向きベクトル取得
		Vector3 muki = pos - Bpos;
		//長さ計算
		float length = Length(muki);

		//範囲外に移動したときキャンセル
		if (length <= minDistance_ && length >= maxDistance_) {
			target_ = nullptr;
		}
#pragma endregion

		
	}

}

void LockOn::Draw() {
	if (target_) {
		lockOn_->Draw();
	}
}

void LockOn::LockOnEnemy(const std::list<Enemy*>& enemies,Camera* camera) {
	//ロックオン状態のカメラ制御
	if (target_) {
		//ターゲット存在時に呼び出されたら処理キャンセル
		target_ = nullptr;
		
	}
	else {

		std::list<std::pair<float, Enemy*>>targetE_;

		//ロックオンする
		for (Enemy* enemy : enemies) {
			//座標取得
			Vector3 pos = enemy->GetWorld().GetMatWorldTranslate();

			//ベース位置取得
			Vector3 Bpos = base_->GetMatWorldTranslate();

			//向きベクトル取得
			Vector3 muki = pos - Bpos;
			//長さ計算
			float length = Length(muki);

			//プレイヤーの向きベクトル計算
			Vector3 offset = { 0,0,1.0f };
			offset = TransformNormal(offset, camera->GetCameraDirectionToFace());
			offset = Normalize(offset);
			//回転量計算
			float yrotate = CheckR_F_Y(Vector2(offset.x,offset.z));

			muki = Normalize(muki);
			float erotate = CheckR_F_Y(Vector2(muki.x, muki.z));

			if (erotate <= yrotate + angleRange_ && erotate >= yrotate - angleRange_) {
				if (length >= minDistance_ && length <= maxDistance_) {
					std::pair<float, Enemy*>ans = std::make_pair(length, enemy);
					targetE_.push_back(ans);
				}
			}
		}

		if (targetE_.size() != 0) {
			targetE_.sort([](auto& pair1, auto& pair2) {return pair1.first < pair2.first; });
			target_ = targetE_.front().second;
			
		}
	}


}
