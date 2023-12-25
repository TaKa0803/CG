#pragma once
#include "IGameObject/GameObject.h"
class Plane :public GameObject {

public:

	void Initialize();

	void DebagWindow();

	void Draw(const Matrix4x4& viewp);


};

