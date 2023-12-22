#include "LockOn.h"
#include"TextureManager.h"

LockOn::~LockOn() {
	delete lockOn_;
}

void LockOn::Initialize() {
	input_ = Input::GetInstance();

	int texture = TextureManager::LoadTex("resources/lockOn.png");
	lockOn_ = Sprite::Create(texture,{64,64});
}


void LockOn::Update(const std::list<Enemy*>& enemies,Camera*camera) {

	//オートロックオン状態の時
	if (isAutoLockOn_) {
		//ターゲットがないとき
		if (!target_) {
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
				offset = TransformNormal(offset, camera->GetMainCamera().matWorld_);
				offset = Normalize(offset);
				//回転量計算
				float yrotate = GetYRotate(Vector2(offset.x, offset.z));

				muki = Normalize(muki);
				float erotate = GetYRotate(Vector2(muki.x, muki.z));

				if (erotate <= yrotate + angleRange_ && erotate >= yrotate - angleRange_) {
					if (length >= minDistance_ && length <= maxDistance_) {
						if (!enemy->GetDead()) {
							std::pair<float, Enemy*>ans = std::make_pair(length, enemy);
							targetE_.push_back(ans);
						}
					}
				}
			}

			if (targetE_.size() != 0) {
				targetE_.sort([](auto& pair1, auto& pair2) {return pair1.first < pair2.first; });
				target_ = targetE_.front().second;

			}
		}
	}



	//マークの処理
	if (target_) {


#pragma region ターゲット変更
		if (input_->IsTriggerButton(kLeft)) {
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
				offset = TransformNormal(offset, camera->GetMainCamera().matWorld_);
				offset = Normalize(offset);
				//回転量計算
				float yrotate = GetYRotate(Vector2(offset.x, offset.z));

				muki = Normalize(muki);
				float erotate = GetYRotate(Vector2(muki.x, muki.z));

				if (erotate <= yrotate && erotate >= yrotate - angleRange_) {
					if (length >= minDistance_ && length <= maxDistance_) {

						if (target_ != enemy) {
							std::pair<float, Enemy*>ans = std::make_pair(length, enemy);
							targetE_.push_back(ans);
						}
					}
				}
			}

			if (targetE_.size() != 0) {
				targetE_.sort([](auto& pair1, auto& pair2) {return pair1.first < pair2.first; });
				target_ = targetE_.front().second;

			}
		}
		if (input_->IsTriggerButton(kRight)) {
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
				offset = TransformNormal(offset, camera->GetMainCamera().matWorld_);
				offset = Normalize(offset);
				//回転量計算
				float yrotate = GetYRotate(Vector2(offset.x, offset.z));

				muki = Normalize(muki);
				float erotate = GetYRotate(Vector2(muki.x, muki.z));

				if (erotate <= yrotate + angleRange_ && erotate >= yrotate) {
					if (length >= minDistance_ && length <= maxDistance_) {
						if (target_ != enemy) {
							std::pair<float, Enemy*>ans = std::make_pair(length, enemy);
							targetE_.push_back(ans);
						}
					}
				}
			}

			if (targetE_.size() != 0) {
				targetE_.sort([](auto& pair1, auto& pair2) {return pair1.first < pair2.first; });
				target_ = targetE_.front().second;

			}
		}
#pragma endregion




		Vector3 pos = target_->GetWorld().GetMatWorldTranslate();

		Matrix4x4 matviewport = MakeViewPortMatrix(0, 0, (float)WindowApp::kClientWidth, (float)WindowApp::kClientHeight, 0, 1);

		Matrix4x4 matVPV = camera->GetViewProjectionMatrix() * matviewport;

		pos = Transform(pos, matVPV);

		//V2に変換
		
		lockOn_->SetPosition(pos);


#pragma region カメラ処理
		//座標取得
		Vector3 tpos = target_->GetWorld().GetMatWorldTranslate();

		//ベース位置取得
		Vector3 Basepos = base_->GetMatWorldTranslate();

		Vector3 offset = tpos - Basepos;
		offset = Normalize(offset);
		//回転量計算
		float yrotate = GetYRotate(Vector2(offset.x, offset.z));

		camera->SetCameraR_Y(yrotate);
#pragma endregion


		
#pragma region キャンセル処理
		//ベース位置取得
		Vector3 Bpos = base_->GetMatWorldTranslate();

		//向きベクトル取得
		Vector3 muki = tpos - Bpos;
		//長さ計算
		float length = Length(muki);

		//範囲外に移動したときキャンセル
		if (length >= minDistance_ && length <= maxDistance_) {
			//範囲内にあるとき
		}
		else {
			target_ = nullptr;
			return;
		}

		//死んだらキャンセル
		if (target_->GetDead()) {
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
			offset = TransformNormal(offset, camera->GetMainCamera().matWorld_);
			offset = Normalize(offset);
			//回転量計算
			float yrotate = GetYRotate(Vector2(offset.x,offset.z));

			muki = Normalize(muki);
			float erotate = GetYRotate(Vector2(muki.x, muki.z));

			if (erotate <= yrotate + angleRange_ && erotate >= yrotate - angleRange_) {
				if (length >= minDistance_ && length <= maxDistance_) {
					if (!enemy->GetDead()) {
						std::pair<float, Enemy*>ans = std::make_pair(length, enemy);
						targetE_.push_back(ans);
					}
				}
			}
		}

		if (targetE_.size() != 0) {
			targetE_.sort([](auto& pair1, auto& pair2) {return pair1.first < pair2.first; });
			target_ = targetE_.front().second;
			
		}
	}


}

void LockOn::Reset() {
	target_ = nullptr;
}
