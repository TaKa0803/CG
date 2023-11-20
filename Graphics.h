#pragma once

#include<string>
#include<d3d12.h>
#include<dxcapi.h>
#include<wrl.h>

class GraphicsSystem
{

public:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	void Initialize(ID3D12Device* device);


	void PreDraw(ID3D12GraphicsCommandList* commandList);
private:

	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	//グラフィックパイプライン
	ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
};

