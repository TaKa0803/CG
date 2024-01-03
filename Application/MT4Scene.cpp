#include "MT4Scene.h"
#include<imgui.h>

void DrawQuaternion(const char* name,const Quaternion&q) {

	ImGui::Begin(name);
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f",q.x,q.y,q.z,q.w);
	ImGui::End();
}



void MT4Scene::Initialize() {

}

void MT4Scene::Update() {

	

	DrawQuaternion("rotation", rotation);
	ImGui::Begin("math");
	ImGui::Text("rotateMatrix");
	
	
	ImGui::Text("%4.2f %4.2f %4.2f : rotateByQuaternion", rotateByQuaternion.x, rotateByQuaternion.y, rotateByQuaternion.z);
	
	ImGui::End();

}

void MT4Scene::Draw() {

}

void MT4Scene::Finalize() {

}
