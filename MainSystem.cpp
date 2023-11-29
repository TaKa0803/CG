#include "MainSystem.h"
#include"InGame.h"
#include"DebugScene.h"
#include"GlobalVariables.h"

MainSystem* MainSystem::GetInstance() {
	static MainSystem instance;
	return &instance;
}

void MainSystem::Run() {
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);

	//エンジン初期化処理
	Initializes();

	//更新処理
	Update();

	//終了処理
	Finalize();

	//終わり
	CoUninitialize();
}

void MainSystem::Initializes() {
	//windowsアプリケーション
	winApp = WinApp::GetInstance();
	winApp->Initialize();

	//DirectX
	DXF = DirectXFunc::GetInstance();
	DXF->Initialize(winApp);

	//画像関係
	textureManager= TextureManager::GetInstance();
	textureManager->InitializeBase(DXF);

	//imgui
	imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(winApp, DXF);

	//入力
	input = Input::GetInstance();
	input->Initialize(winApp);


}

void MainSystem::Update() {

	GlobalVariables::GetInstance()->LoadFiles();

	InGame* ingame = new InGame();
	ingame->Initialize();

	//DebugScene* dScene = new DebugScene();
	//dScene->Initialize();

#pragma region 更新
	while (winApp->ProcessMessage()) {
#pragma region 状態更新
		///更新前処理
		//ImGui
		imguiManager->PreUpdate();

		//キー入力
		input->Update();
		///=以下更新=//

		GlobalVariables::GetInstance()->Update();

		ingame->Update();
		//dScene->Update();


		//開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に書き換える
		ImGui::ShowDemoWindow();


		//==更新終わり==//
		//更新終わり描画前処理
		imguiManager->PostUpdate();
#pragma endregion
#pragma region 描画		
		///描画前処理
		//DirectX
		DXF->PreDraw();
		//ImGui
		imguiManager->PreDraw();
		
		//==以下描画==//


		ingame->Draw();
		//dScene->Draw();

		//==描画終わり==//

		///描画あと処理
		//imGui
		imguiManager->PostDraw();
		//DirectX
		DXF->PostDraw();
#pragma endregion
	}
#pragma endregion

	ingame->Finalize();
	//dScene->Finalize();
}

void MainSystem::Finalize() {
	///開放処理
	textureManager->Finalize();
	imguiManager->Finalize();
	DXF->Finalize();
	winApp->Finalize();

}
