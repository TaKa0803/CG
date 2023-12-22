#pragma once
#include"IGameObject.h"

class Player :public IGameObject {

public:
	Player();
	~Player();

	void Initialize();

	void Update();

	void Draw(const Matrix4x4&viewprojection);

	void DebugWindow(const char*name);


private:



};