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

	rotateMatrix = MakeRotateAxisAngle(axis, angle);

	ImGui::Begin("rotateMatrix");
	ImGui::Text("%4.3f ,%4.3f ,%4.3f ,%4.3f ", rotateMatrix.m[0][0], rotateMatrix.m[0][1], rotateMatrix.m[0][2], rotateMatrix.m[0][3]);
	ImGui::Text("%4.3f ,%4.3f ,%4.3f ,%4.3f ", rotateMatrix.m[1][0], rotateMatrix.m[1][1], rotateMatrix.m[1][2], rotateMatrix.m[1][3]);
	ImGui::Text("%4.3f ,%4.3f ,%4.3f ,%4.3f ", rotateMatrix.m[2][0], rotateMatrix.m[2][1], rotateMatrix.m[2][2], rotateMatrix.m[2][3]);
	ImGui::Text("%4.3f ,%4.3f ,%4.3f ,%4.3f ", rotateMatrix.m[3][0], rotateMatrix.m[3][1], rotateMatrix.m[3][2], rotateMatrix.m[3][3]);
	ImGui::End();


}

void InGame::Draw() {



}
void InGame::Finalize() {

}

