#pragma once

#include"WinApp/WinApp.h"
#include"DirectXFunc/DirectXFunc.h"
#include"Graphics/Graphics.h"
#include"TextureManager/TextureManager.h"
#include"ModelManager/ModelManager.h"
#include"ImGuiManager/ImGuiManager.h"
#include"Input/Input.h"
#include"SRVManager/SRVManager.h"
#include"RandomNum/RandomNum.h"



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

	//モデル
	InstancingModelManager* instancingModel_;

	//入力
	Input* input;

	//SRV
	SRVManager* SRVM_;

	RandomNumber* randomNumClass_;
#pragma endregion
};