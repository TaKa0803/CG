#pragma once
#include<string>
#include<vector>

#include<array>

#include"externals/DirectXTex/d3dx12.h"

#include"externals/DirectXTex/DirectXTex.h"
#include"DirectXFunc.h"

#include<wrl.h>


class TextureManager {
public://シングルトンパターン
	static TextureManager *GetInstance();

private:
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager& o) = delete;
	const TextureManager& operator=(const TextureManager& o) = delete;

public:

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// 画像の読み込み処理
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	static int LoadTex(const std::string& filePath);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="DXF_"></param>
	void Initialize(DirectXFunc* DXF_);
	
	void Finalize();
		
private://メンバ関数
	
	int CreateData(const DirectX::ScratchImage& mipImages);

private://メンバ変数

	//
	DirectXFunc* DXF;

	

	struct Texture{
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
	};

	
	
};