#pragma once

#include"WinApp.h"
#include"DirectXFunc.h"
#include"Graphics.h"
#include"TextureManager.h"
#include"ImGuiManager.h"
#include"Input.h"
#include"SRVManager.h"

class MainSystem {
public://シングルトンパターン
	static MainSystem* GetInstance();

private://シングルトンパターン

	MainSystem() = default;
	~MainSystem() = default;
	MainSystem(const MainSystem& o) = delete;
	const MainSystem& operator=(const MainSystem& o) = delete;

public:

	void Run();

private:
	void Initializes();

	void MainRoop();

	void Finalize();

private:
#pragma region 基板初期化
	//windowsアプリケーション
	WindowApp* winApp;

	//DirectX
	DirectXFunc* DXF;

	//ImGui
	ImGuiManager* imguiManager;

	//texture
	TextureManager* textureManager;

	//入力
	Input* input;

	//SRV
	SRVManager* SRVM_;
#pragma endregion
};