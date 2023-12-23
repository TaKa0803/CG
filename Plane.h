#pragma once
#include "IGameObject.h"
class Plane :public IGameObject {

public:

	void Initialize();

	void DebagWindow();

	void Draw(const Matrix4x4& viewp);


};

