#pragma once
#include<string>
#include<vector>
#include"externals/DirectXTex/d3dx12.h"

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

	int AddtextureNum(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU);


	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureDescriptorHandle(int num);

	int GetDataSize() { return (int)textureData_.size(); }

private://メンバ関数
	//Textureデータを読む
	//DirectX::ScratchImage LoadTexture(const std::string& filePath);


private://メンバ変数
	struct Texture{
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
	};

	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureData_;
	
};