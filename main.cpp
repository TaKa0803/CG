#include"DirectXFunc.h"
#include"WinApp.h"
#include"ImGuiManager.h"
#include"Graphics.h"
#include"TextureManager.h"


#include"Model.h"
#include"Sprite.h"
#include"Input.h"

#include<dxgidebug.h>

#include"InGame.h"


#pragma region 構造体
struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {
		//リソースリークチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1>debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			//警告時に止まる
		}
	}
};
#pragma endregion



//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//リークチェック
	static D3DResourceLeakChecker lackCheck;

	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);


#pragma region 基板初期化
	//windowsアプリケーション
	WinApp* winApp = WinApp::GetInstance();
	winApp->Initialize();
	
	//DirectX
	DirectXFunc* DXF = DirectXFunc::GetInstance();
	DXF->Initialize(winApp);
	
	//Graphic関係
	GraphicsSystem* graphics = GraphicsSystem::GetInstance();	
	graphics->Initialize(DXF->GetDevice());
	
	//画像関係
	TextureManager::GetInstance()->Initialize(DXF);

	//ImGui
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(winApp, DXF);

	//入力
	Input* input = Input::GetInstance();
	input->Initialize(winApp);
#pragma endregion


	InGame* ingame = new InGame();
	ingame->Initialize();






	

#pragma region 更新
	while (winApp->ProcessMessage()) {
#pragma region 状態更新
		///更新前処理
		//ImGui
		imguiManager->PreUpdate();

		//キー入力
		input->Update();
		///=以下更新=//

		ingame->Update();



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
		//Graphics
		graphics->PreDraw(DXF->GetCMDList());

		//==以下描画==//


		ingame->Draw();


		//==描画終わり==//

		///描画あと処理
		//imGui
		imguiManager->PostDraw();
		//DirectX
		DXF->PostDraw();
#pragma endregion
	}
#pragma endregion

	///開放処理
	imguiManager->Finalize();	
	DXF->Finalize();
	winApp->Finalize();

	//終わり
	CoUninitialize();
	return 0;
}