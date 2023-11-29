#include"InGame.h"
#include"TextureManager.h"
#include"externals/Imgui/imgui.h"
#include<cmath>
#include<iostream>


//AABB同士
bool InCollision(const AABB& a, const AABB& b) {

	if ((a.minV.x <= b.maxV.x && a.maxV.x >= b.minV.x) &&//x軸
		(a.minV.y <= b.maxV.y && a.maxV.y >= b.minV.y) &&
		(a.minV.z <= b.maxV.z && a.maxV.z >= b.minV.z)
		) {
		return true;
	}
	return false;
}



void InGame::Initialize() {

	



	




}

void InGame::Update() {
	Matrix4x4 rotateM0 = DirectionToDirection(Normalize(Vector3{ 1.0f,0.0f,0.0f }), Normalize(Vector3{ -1.0f,0.0f,0.0f }));

	Matrix4x4 rotateM1 = DirectionToDirection(from0, to0);
	Matrix4x4 rotateM2 = DirectionToDirection(from1, to1);


	Matrix4x4Debug(rotateM0,"ro0");
	Matrix4x4Debug(rotateM1, "ro1");
	Matrix4x4Debug(rotateM2, "ro2");

}

void InGame::Draw() {

	
}
void InGame::Finalize() {

}
