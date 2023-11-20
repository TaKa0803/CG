#pragma once
#include<string>
#include<vector>

#include<array>

#include"externals/DirectXTex/d3dx12.h"

#include"externals/DirectXTex/DirectXTex.h"
#include"DirectXFunc.h"

#include<wrl.h>


class TextureManager {
public:
	static TextureManager *GetInstance();

private://シングルトンパターン

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager& o) = delete;
	const TextureManager& operator=(const TextureManager& o) = delete;



public:


	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	static int LoadTex(const std::string& filePath);

	void InitializeBase(DirectXFunc* DXF_);
	
	void Finalize();


	int AddtextureNum(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU);


	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureDescriptorHandle(int num);

	int GetDataSize() { return (int)textureData_.size(); }

	
private://メンバ関数
	


	ID3D12Resource* PushTextureResource(ID3D12Resource*resource);
	
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPU_DES_HANDLE();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPU_DES_HANDLE();


	
	
	int Initialize(DirectX::ScratchImage& mipImages);

private://メンバ変数

	//
	DirectXFunc* DXF;

	

	struct Texture{
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
	};

	std::vector<ID3D12Resource*>textureResources_;

	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureData_;
	
};