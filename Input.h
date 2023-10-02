#pragma once

#include<wrl.h>
#define DIRECTINPUT_VERSION 0x0800 //DirectInputのバージョン指定
#include<dinput.h>
#include"WinApp.h"


//入力関数
class Input {

public:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;


public:
	//初期化
	void Initialize(WinApp*winApp);
	//更新
	void Update();

	
/// <summary>
///		キー入力チェック
/// </summary>
/// <param name="keyNumber">キー番号(DIK)</param>
///	<returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);

	/// <summary>
	///		キーのトリガーチェック
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK)</param>
	///	<returns>トリガーされたか</returns>
	bool TriggerKey(BYTE keyNumber);


private:
	//DirectInput
	ComPtr<IDirectInput8>directInput = nullptr;
	//キーボードデバイス
	ComPtr<IDirectInputDevice8>keyboard;
	//キー
	BYTE key[256] = {};
	BYTE keyPre[256] = {};

	//WindowsAPI
	WinApp* winApp_ = nullptr;
};