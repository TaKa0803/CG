#pragma once

#include<Windows.h>
#include<cstdint>
#include<d3d12.h>


class WinApp {
public:	//静的メンバ変数
	
	static LRESULT CALLBACK WindowProc(HWND hwnd_, UINT msg, WPARAM wparam, LPARAM lparam);

public://定数

	//ウィンドウ横幅
	static const int window_width = 1280;
	//ウィンドウ縦幅
	static const int window_heigth = 720;


public:

	// 初期化
	void Initialize();
	// 更新
	void Update();

	//終了
	void Finalize();


	/// <summary>
	/// ウィンドウメッセージ受け取りで更新終了
	/// </summary>
	/// <returns>true:実行 false:終了</returns>
	bool ProcessMessage();

	//ウィンドウハンドルのゲッター
	HWND GetHwnd() const { return hwnd_; };
	//ウィンドウクラスのゲッター
	HINSTANCE GetHInstace() const { return wc.hInstance; };

private:
	//ウィンドウクラスの設定
	WNDCLASS wc{};

	//ウィンドウハンドル
	HWND hwnd_=nullptr;

	//デバッグレイヤー
	ID3D12Debug1* debugController = nullptr;
};