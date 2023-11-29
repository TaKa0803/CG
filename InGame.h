#pragma once
#include"Input.h"
#include"struct.h"

#include<iostream>
#include"Matrix.h"
#include"Vector3.h"




class InGame {
public:
	void Initialize();

	void Update();

	void Draw();

	void Finalize();
private:
	
	Vector3 from0 = Normalize(Vector3{ 1.0f,0.7f,0.5f });
	Vector3 to0 = -from0;

	Vector3 from1 = Normalize(Vector3{ -0.6,0.9f,0.2f });
	Vector3 to1 = Normalize(Vector3{0.4f,0.7f,-0.5f});

	Matrix4x4 rotateM0 = DirectionToDirection(Normalize(Vector3{ 1.0f,0.0f,0.0f }), Normalize(Vector3{ -1.0f,0.0f,0.0f }));

	Matrix4x4 rotateM2 = DirectionToDirection(from0, to0);
	Matrix4x4 rotateM3 = DirectionToDirection(from1, to1);

};