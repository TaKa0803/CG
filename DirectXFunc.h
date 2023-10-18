#pragma once
#include"WinApp.h"
#include<d3d12.h>
#include<dxgi1_6.h>

#include<stdint.h>

#include<wrl.h>
using namespace Microsoft::WRL;


class DirectXFunc {
public://シングルトンパターン
	static DirectXFunc* GetInstance();

private://シングルトンパターン

	DirectXFunc() = default;
	~DirectXFunc() = default;
	DirectXFunc(const DirectXFunc& o) = delete;
	const DirectXFunc& operator=(const DirectXFunc& o) = delete;


public:	//静的メンバ変数

	

public:
	/// <summary>
	/// イニシャライズ
	/// </summary>
	/// <param name="winApp"></param>
	void Initialize(WinApp* winApp);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// キックコマンド
	/// </summary>
	void KickCommand();

	
	void Finalize();

#pragma region ゲッター
	ID3D12Device* GetDevice()const { return device.Get(); }

	ID3D12GraphicsCommandList* GetCMDList()const { return commandList.Get();}

	DXGI_SWAP_CHAIN_DESC1 GetswapChainDesc()const { return swapChainDesc; }

	D3D12_RENDER_TARGET_VIEW_DESC GetrtvDesc()const { return rtvDesc; }
#pragma endregion

	


#pragma region クラス内関数
	void D3D12Devicenitialize();

	void CommandInitialize();

	void SwapChainInitialize();

	void RTVInitialize();

	void DSVInitialize();

	
	void FenceInitialize();
#pragma endregion




	//ポインタ
	WinApp* winApp_ = nullptr;



	//dxgiファクトリー
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	//デバイス
	ComPtr<ID3D12Device> device = nullptr;
	//コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	//コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	//スワップチェーン
	ComPtr<IDXGISwapChain4> swapChain = nullptr;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	//スワップチェーンリソース
	ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };


	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	ID3D12DescriptorHeap* dsvDescriptorHeap;
	uint32_t descriptorSizeDSV;
	//フェンス
	ID3D12Fence* fence = nullptr;
	uint32_t fenceValue = 0;
	//イベント
	HANDLE fenceEvent;

	//バリア
	D3D12_RESOURCE_BARRIER barrier{};


};