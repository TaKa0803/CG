#pragma once

#include<dxgi1_6.h>
#include<d3d12.h>
#include<wrl.h>

#include"WinApp.h"

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);
D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);


//dx関連
class DX
{
public:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;


	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="winApp"></param>
	void Initialize(WinApp* winApp);

	

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	//描画後処理
	void PostDraw();

	//キックコマンド
	void KickCommand();


	void Finalize();

	ID3D12Device* GetDevice()const { return device.Get(); }

	ID3D12GraphicsCommandList* GetCommand()const { return commandList.Get(); }

private:
#pragma region private関数
	/// <summary>
	/// デバイスの初期化
	/// </summary>
	void InitializeDevice();

	/// <summary>
	/// コマンド初期化
	/// </summary>
	void InitializeCommand();

	/// <summary>
	/// スワップチェイン初期化
	/// </summary>
	void InitializeSwapChain();

	/// <summary>
	/// レンダーターゲットビュー初期化
	/// </summary>
	void InitializeRTV();

	/// <summary>
	/// 震度バッファ初期化
	/// </summary>
	void InitializeDSV();

	/// <summary>
	/// フェンスの初期化
	/// </summary>
	void InitializeFence();
#pragma endregion

	
	//アプリのポインタ
	WinApp* winApp_ = nullptr;

	//デバイス
	ComPtr<ID3D12Device> device = nullptr;
	//DXGIファクトリー
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	//コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	//コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	//スワップチェイン
	ComPtr<IDXGISwapChain4> swapChain = nullptr;

	ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };

#pragma region 震度バッファ
	//DSV用のヒープでディスクリプタの数は１。DSVはShader内で触るものではないので、ShaderVisibleはfalse
#pragma endregion
	ID3D12DescriptorHeap* dsvDescriptorHeap;
#pragma endregion
	//RTC
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];


	//フェンス
	ComPtr<ID3D12Fence> fence = nullptr;
	uint32_t fenceValue = 0;
	//イベント
	HANDLE fenceEvent;

	D3D12_RESOURCE_BARRIER barrier{};
};

