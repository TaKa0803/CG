#include "MainSystem.h"
#include"InGame.h"
#include"DebugScene.h"
#include"MT4Scene.h"
#include"GlobalVariables.h"
#include"RandomNum.h"
#include"ModelManager.h"

MainSystem* MainSystem::GetInstance() {
	static MainSystem instance;
	return &instance;
}

void MainSystem::Run() {
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);

	//エンジン初期化処理
	Initializes();

	//更新処理
	MainRoop();

	//終了処理
	Finalize();

	//終わり
	CoUninitialize();
}

void MainSystem::Initializes() {
	//windowsアプリケーション
	winApp = WindowApp::GetInstance();
	winApp->Initialize();

	//DirectX
	DXF = DirectXFunc::GetInstance();
	DXF->Initialize(winApp);

	//SRV
	SRVM_ = SRVManager::GetInstance();
	SRVM_->Initialize(DXF);
	
	//画像関係
	textureManager= TextureManager::GetInstance();
	textureManager->Initialize(DXF);
	
	//imgui
	imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(winApp, DXF);

	//入力
	input = Input::GetInstance();
	input->Initialize(winApp);


	//モデルデータ関係
	ModelManager* mManager = ModelManager::GetInstance();
	mManager->LoadAllModels();


	//乱数クラス
	randomNumClass_ = RandomNumber::GetInstance();
	randomNumClass_->RandomNumberProcessInitialize();
	
}

void MainSystem::MainRoop() {

	GlobalVariables::GetInstance()->LoadFiles();

	//InGame* ingame = new InGame();
	//ingame->Initialize();

	//DebugScene* dScene = new DebugScene();
	//dScene->Initialize();

	MT4Scene* mt4 = new MT4Scene();
	mt4->Initialize();


	SRVM_->PostInitialize();
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

		//ingame->Update();
		//dScene->Update();
		mt4->Update();

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


		//ingame->Draw();
		//dScene->Draw();
		mt4->Draw();

		//==描画終わり==//

		///描画あと処理
		//imGui
		imguiManager->PostDraw();
		//DirectX
		DXF->PostDraw();
#pragma endregion
	}
#pragma endregion

	//ingame->Finalize();
	//dScene->Finalize();
	mt4->Finalize();
}

void MainSystem::Finalize() {
	///開放処理
	SRVM_->Finalize();
	textureManager->Finalize();
	imguiManager->Finalize();
	DXF->Finalize();
	winApp->Finalize();

}
