#include"Input.h"


#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"Xinput9_1_0.lib")


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


	XInputGetState(0, &joyState);
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

bool Input::IsControllerActive() {

	if (XInputGetState(0, &joyState)==ERROR_SUCCESS) {
		return true;
	}
	
	return false;
}

Vector2 Input::GetjoyStickR() {
	Vector2 r= {
		(float)joyState.Gamepad.sThumbRX / SHRT_MAX,
		(float)joyState.Gamepad.sThumbRY / SHRT_MAX,
	};
	if (r.x > 0 && r.x < deadLine_) {
		r.x = 0;
	}
	if (r.x < 0 && r.x > -deadLine_) {
		r.x = 0;
	}

	if (r.y > 0 && r.y < deadLine_) {
		r.y = 0;
	}
	if (r.y < 0 && r.y > -deadLine_) {
		r.y = 0;
	}

	return r;
}

Vector2 Input::GetjoyStickL() {
	Vector2 r = {
		(float)joyState.Gamepad.sThumbLX / SHRT_MAX,
		(float)joyState.Gamepad.sThumbLY / SHRT_MAX,
	};
	if (r.x > 0 && r.x < deadLine_) {
		r.x = 0;
	}else if (r.x < 0 && r.x > -deadLine_) {
		r.x = 0;
	}

	if (r.y > 0 && r.y < deadLine_) {
		r.y = 0;
	}else if (r.y < 0 && r.y > -deadLine_) {
		r.y = 0;
	}

	return r;
}
