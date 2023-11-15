#pragma once
#include"Input.h"
#include"struct.h"

#include<iostream>
#include"Matrix.h"





class InGame {
public:
	void Initialize();

	void Update();

	void Draw();

	void Finalize();
private:
	
	Vector3 axis = Normalize({ 1.0f,1.0f,1.0f });

	float angle = 0.44f;

	Matrix4x4 rotateMatrix;
};