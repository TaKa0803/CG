#pragma once
#include"WinApp.h"
#include<d3d12.h>
#include<dxgi1_6.h>

#include<stdint.h>



class DirectXFunc {
public://シングルトンパターン
	static DirectXFunc* GetInstance();

private://シングルトンパターン

	DirectXFunc() = default;
	~DirectXFunc() = default;
	DirectXFunc(const DirectXFunc& o) = delete;
	const DirectXFunc& operator=(const DirectXFunc& o) = delete;


public:	//静的メンバ変数

	static ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	static ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);


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
	ID3D12Device* GetDevice()const { return device; }

	ID3D12GraphicsCommandList* GetCMDList()const { return commandList;}

	DXGI_SWAP_CHAIN_DESC1 GetswapChainDesc()const { return swapChainDesc; }

	D3D12_RENDER_TARGET_VIEW_DESC GetrtvDesc()const { return rtvDesc; }
#pragma endregion

	ID3D12DescriptorHeap* GetSRV()const { return srvDescriptorHeap; }
	uint32_t GetSRVsize()const { return descriptorSizeSRV; }



#pragma region クラス内関数
	void D3D12Devicenitialize();

	void CommandInitialize();

	void SwapChainInitialize();

	void RTVInitialize();

	void DSVInitialize();

	
	void FenceInitialize();
#pragma endregion

#pragma region 仮移動
	void SRVInitialize();
	//SRV用のヒープでディスクリプタの数は１２８。SRVはSHADER内で触るものなので、ShaderVisibleはtrue
	ID3D12DescriptorHeap* srvDescriptorHeap;
	uint32_t descriptorSizeSRV;

	
#pragma endregion


	//ポインタ
	WinApp* winApp_ = nullptr;



	//dxgiファクトリー
	IDXGIFactory7* dxgiFactory = nullptr;
	//デバイス
	ID3D12Device* device = nullptr;
	//コマンドキュー
	ID3D12CommandQueue* commandQueue = nullptr;
	//コマンドアロケータ
	ID3D12CommandAllocator* commandAllocator = nullptr;
	//コマンドリスト
	ID3D12GraphicsCommandList* commandList = nullptr;
	//スワップチェーン
	IDXGISwapChain4* swapChain = nullptr;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	//スワップチェーンリソース
	ID3D12Resource* swapChainResources[2] = { nullptr };


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