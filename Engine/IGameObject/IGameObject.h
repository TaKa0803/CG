#pragma once
#include"Model/Model.h"
#include"WorldTransform/WorldTransform.h"
#include<vector>

class GameObject {

protected:

	Model* model_=nullptr;

	WorldTransform world_;

	
public:
	const WorldTransform& GetWorld() { return world_; }

	virtual ~GameObject();

	virtual void Initialize(const std::string&filepath );

	virtual void Update();

	virtual void Draw(const Matrix4x4&viewProjection);


};

