#include "Instancing/InstancingGameObject.h"

InstancingGameObject::~InstancingGameObject() {
}

void InstancingGameObject::Initialize(const std::string& tag) {
	IMM_ = InstancingModelManager::GetInstance();

	//タグが存在しているかチェック
	if (IMM_->SerchTag(tag)) {
		tag_ = tag;
	}
}

void InstancingGameObject::Update() {
	//行列更新
	world_.UpdateMatrix();
	//タグに対応したモデルにワールド追加
	IMM_->SetWorld(tag_, world_);
}


