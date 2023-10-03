#include"WinApp.h"

//ウィンドウプロシーシャ
LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
#pragma region 
	/*
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	*/
#pragma endregion

	//メッセージに対してゲーム固有の処理をする
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}


void WinApp::Initialize()
{
#pragma region ウィンドウ生成 
	//ウィンドウクラスの設定	
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = L"A";
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&wc);

	
	RECT wrc = { 0,0,window_width,window_heigth };

	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);


	hwnd_ = CreateWindow(
		wc.lpszClassName,
		L"CG2",					//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr);

#pragma region DebugLayer
#ifdef _DEBUG
	
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバッグレイヤーを有効化
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif
#pragma endregion

	ShowWindow(hwnd_, SW_SHOW);


	
#pragma endregion
}

void WinApp::Update()
{
}

void WinApp::Finalize()
{
#ifdef _DEBUG
	debugController->Release();
#endif // _DEBUG
	CloseWindow(hwnd_);
}

bool WinApp::ProcessMessage()
{

	MSG msg{};
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT) {
		return false;
	}

	return true;
}


