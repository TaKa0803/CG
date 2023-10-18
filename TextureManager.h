#pragma once
#include<string>
#include<vector>
#include"externals/DirectXTex/d3dx12.h"

#include"DirectXFunc.h"

#include<wrl.h>
using namespace Microsoft::WRL;

class TextureManager {
public:
	static TextureManager *GetInstance();

private://シングルトンパターン

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager& o) = delete;
	const TextureManager& operator=(const TextureManager& o) = delete;



public:

	static int LoadTex(const std::string& filePath);

	void Initialize(DirectXFunc* DXF_);
	

	int AddtextureNum(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU);


	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureDescriptorHandle(int num);

	int GetDataSize() { return (int)textureData_.size(); }

	ID3D12DescriptorHeap* GetSRV() { return srvDescriptorHeap.Get(); }

private://メンバ関数
	


	void SRVInitialize();
	
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPU_DES_HANDLE();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPU_DES_HANDLE();


	//SRV用のヒープでディスクリプタの数は１２８。SRVはSHADER内で触るものなので、ShaderVisibleはtrue
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
	uint32_t descriptorSizeSRV;

	//
	DirectXFunc* DXF;

private://メンバ変数
	struct Texture{
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
	};

	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureData_;
	
};