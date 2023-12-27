#pragma once

#include"InstancingModelManager/InstancingModelManager.h"
#include"WorldTransform/WorldTransform.h"
#include<vector>

class InstancingGameObject {

protected:

	std::string tag_;

	WorldTransform world_;

	InstancingModelManager* IMM_;

public:
	const WorldTransform& GetWorld() { return world_; }

	virtual ~InstancingGameObject();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="tag">モデルタグ</param>
	virtual void Initialize(const std::string& tag);

	/// <summary>
	/// 行列とワールド追加
	/// </summary>
	virtual void Update();


};

