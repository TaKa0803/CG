#pragma once

#include"WinApp.h"
#include"DirectXFunc.h"

class ImGuiManager
{
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

};

