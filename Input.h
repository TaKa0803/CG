#pragma once

#include<wrl.h>

#define DIRECTINPUT_VERSION 0x0800 //バージョン設定
#include<dinput.h>


#include"WinApp.h"



class Input {
public:

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	static Input GetInstance();

	void Initialize(WinApp*winApp);

	void Update();
private:

	//キーボードデバイス
	ComPtr<IDirectInputDevice8>keyboad;

};