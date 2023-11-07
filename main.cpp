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
	static D3DResourceLeakChecker lackCheck;


	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);


#pragma region 基板初期化
	WinApp* winApp = WinApp::GetInstance();
	winApp->Initialize();

	DirectXFunc* DXF = DirectXFunc::GetInstance();
	DXF->Initialize(winApp);

	//GraphicsSystem *graphics = nullptr;
	//graphics = new GraphicsSystem;
	//graphics->Initialize(DXF->GetDevice());



	//TextureManager::GetInstance()->Initialize(DXF);


	//Input* input = Input::GetInstance();
	//input->Initialize(winApp);

#pragma endregion


	//InGame* ingame = new InGame();
	//ingame->Initialize();






	//ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	//imguiManager->Initialize(winApp, DXF);

#pragma region 更新

	while (winApp->ProcessMessage()) {
		//imguiManager->PreUpdate();
		//ゲーム内処理
		//input->Update();


		//ingame->Update();



		//開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に書き換える
		//ImGui::ShowDemoWindow();



		//imguiManager->PostUpdate();
#pragma region 描画		
#pragma region コマンドを積み込んで確定させる
		DXF->PreDraw();
		//imguiManager->PreDraw();
		//graphics->PreDraw(DXF->GetCMDList());




		//ingame->Draw();




		//imguiManager->PostDraw();
		DXF->PostDraw();
#pragma endregion
#pragma endregion

	}
#pragma endregion







	//imguiManager->Finalize();	
	DXF->Finalize();
	winApp->Finalize();





	CoUninitialize();
	return 0;
}