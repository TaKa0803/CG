#pragma once

#include<wrl.h>

#define DIRECTINPUT_VERSION 0x0800 //バージョン設定
#include<dinput.h>
#include <XInput.h>
#include"Vector2.h"
#include"WinApp.h"



class Input {
public:

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	static Input* GetInstance();

	void Initialize(WinApp*winApp);

	void Update();

	bool PushKey(BYTE keyNum);

	bool TriggerKey(BYTE keyNum);



	bool IsControllerActive();

	Vector2 GetjoyStickR();

	Vector2 GetjoyStickL();

	void SetDeadLine(float deadLine) { deadLine_ = deadLine; }
private:

	//キーボードデバイス
	ComPtr<IDirectInputDevice8>keyboad;

	BYTE key[256] = {};

	BYTE preKey[256] = {};

	XINPUT_STATE joyState;
	//デッドライン
	float deadLine_ = 0.1f;
};