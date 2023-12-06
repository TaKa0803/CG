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

	DrawQuaternion("identity", identity);
	DrawQuaternion("conjugate", conj);
	DrawQuaternion("inverse", inv);
	DrawQuaternion("Normakize", normal);
	DrawQuaternion("multi1", mul1);
	DrawQuaternion("multi2", mul2);
	ImGui::Begin("Norm");
	ImGui::Text("%4.1f", norm);
	ImGui::End();

}

void MT4Scene::Draw() {

}

void MT4Scene::Finalize() {

}
