#pragma once

#include"Model/Model.h"
#include"WorldTransform/WorldTransform.h"
#include<vector>

class InstancingGameObject {

protected:

	InstancingModel* model_ = nullptr;

	WorldTransform world_;


public:
	const WorldTransform& GetWorld() { return world_; }

	virtual ~InstancingGameObject();

	virtual void Initialize(const std::string& filepath);

	virtual void Update();

	virtual void Draw();


};

