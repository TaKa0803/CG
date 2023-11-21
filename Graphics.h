#pragma once

#include<string>
#include<d3d12.h>
#include<dxcapi.h>
#include<wrl.h>


enum class BlendMode {
	//!<ブレンドなし
	kNone,
	//!<通常αブレンド
	kNormal,
	//!<加算
	kAdd,
	//!<減算
	kSubtract,
	//!<乗算
	kMultily,
	//!<スクリーン
	kScreen,
	//利用してはいけない
	kCountOfBlendMode
};

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

