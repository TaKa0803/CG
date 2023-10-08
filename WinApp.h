#pragma once

#include<Windows.h>


class WinApp {
public://シングルトンパターン
	static WinApp* GetInstance();

private://シングルトンパターン

	WinApp() = default;
	~WinApp() = default;
	WinApp(const WinApp& o) = delete;
	const WinApp& operator=(const WinApp& o) = delete;


public://静的メンバ変数
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
		
	//
	static const int kClientWidth = 1280;
	static const int kClientHeight = 720;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	
	/// <summary>
	/// 消す前の処理
	/// </summary>
	void Finalize();
/// <summary>
/// プロセスメッセージ
/// </summary>
	bool ProcessMessage();

	HWND GetHwnd()const { return hwnd_; }
private:

	//ウィンドウハンドル
	HWND hwnd_ = nullptr;
};