#pragma once
#include"Model.h"
#include"WorldTransform.h"
#include"Camera.h"

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
};

