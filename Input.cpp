#include"Input.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace Microsoft::WRL;
#include<string.h>
#include<cassert>

void Input::Initialize(WinApp* winApp)
{
	this->winApp_ = winApp;

	//DirectInputインスタンス生成	
	HRESULT hr = DirectInput8Create(winApp_->GetHInstace(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(hr));
	//キーボードデバイス生成
	hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	//入力データ形式のセット
	hr = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	//排他制御レベルのセット
	hr = keyboard->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
}

void Input::Update()
{
	HRESULT hr;

	//前回のキー入力を保存
	memcpy(keyPre, key, sizeof(key));

	hr=keyboard->Acquire();
	assert(SUCCEEDED(hr));
	hr=keyboard->GetDeviceState(sizeof(key), key);
	assert(SUCCEEDED(hr));


}

bool Input::PushKey(BYTE keyNumber)
{
	//押されている
	if (key[keyNumber]) {
		return true;
	}
	//押されていない
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	//前回押されておらず今回押されたら
	if (key[keyNumber] && !keyPre[keyNumber]) {
		return true;
	}
	return false;
}
