#pragma once
#include"WorldTransform.h"
#include"Model.h"

class BoxColider {
public:

	~BoxColider();

	void Initialize(const WorldTransform* world=nullptr);

	
	void Update();

	void Draw(const Matrix4x4& VP);


public://ゲッター
#pragma region ゲッター
	const WorldTransform& GetWorld() { return world_; }
#pragma endregion


public://セッター

public://メンバ変数
	WorldTransform world_;
private:
	Model* model_;
	int texture_;
};

