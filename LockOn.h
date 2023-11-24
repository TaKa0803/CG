#pragma once

#include"Sprite.h"

class LockOn {

public:

	~LockOn();

	void Initialize();

	void Update();

	void Draw();

private:
	Sprite* lockOn_;
};

