#include "LockOn.h"
#include"TextureManager.h"


LockOn::~LockOn() {
	delete lockOn_;
}

void LockOn::Initialize() {
	int texture = TextureManager::LoadTex("resources/lockOn.png");
	lockOn_ = Sprite::Create(texture,{64,64});
}

void LockOn::Update() {
	
}

void LockOn::Draw() {
	lockOn_->Draw();
}
