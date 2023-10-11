#pragma once

#include"WinApp.h"
#include"DirectXFunc.h"
#include"TextureManager.h"

#include"externals/imgui/imgui.h"

class ImGuiManager
{
public:
	static ImGuiManager* GetInstance();

	static const size_t kNumDescriptors = 256;


private://シングルトンパターン

	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager& o) = delete;
	const ImGuiManager& operator=(const ImGuiManager& o) = delete;


public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApp"></param>
	void Initialize(WinApp* winApp, DirectXFunc* DXF);

	/// <summary>
	/// 更新最初に処理
	/// </summary>
	void PreUpdate();

	/// <summary>
	/// 更新後描画前に処理
	/// </summary>
	void PostUpdate();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// 開放処理
	/// </summary>
	void Finalize();

private:
	//ポインタ
	WinApp* winApp_ = nullptr;
	DirectXFunc* DXF_ = nullptr;
	TextureManager* textureManager_ = nullptr;
};

