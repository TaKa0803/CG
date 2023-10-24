#include"Input.h"


#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include<cassert>

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

void Input::Initialize(WinApp*winApp)
{

	
	//インスタンス生成
	ComPtr<IDirectInput8>directInput = nullptr;
	HRESULT hr = DirectInput8Create(winApp->GetWc().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(hr));
	//キーボードデバイス
	
	hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboad, NULL);
	assert(SUCCEEDED(hr));

	//入力データ形式リセット
	hr = keyboad->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));

	//排他制御レベルのセット
	hr = keyboad->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

}

void Input::Update()
{

	memcpy(preKey, key, sizeof(key));

	//キーボード情報取得開始
	keyboad->Acquire();

	
	keyboad->GetDeviceState(sizeof(key), key);

}

bool Input::PushKey(BYTE keyNum) {
	if (key[keyNum]) {
		return true;
	}
	return false;
}

bool Input::TriggerKey(BYTE keyNum) {

	if (key[keyNum] && !preKey[keyNum]) {
		return true;
	}

	return false;
}
