#pragma once
#include"Model.h"
#include"WorldTransform.h"
#include<vector>

class IGameObject {

protected:

	Model* model_=nullptr;

	WorldTransform world_;

public:

	virtual ~IGameObject();

	virtual void Initialize(const std::string&filepath );

	virtual void Update();

	virtual void Draw(const Matrix4x4&viewProjection);


};

