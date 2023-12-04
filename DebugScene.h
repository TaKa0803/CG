#pragma once
#include"Model.h"
#include"WorldTransform.h"
#include"Camera.h"
#include"Sprite.h"

class DebugScene {
public:
	void Initialize();

	void Update();

	void Draw();

	void Finalize();

private:

	Camera camera_;

	Model* model_;
	WorldTransform world_;
	int texture;

	Sprite* sprite_;

};

