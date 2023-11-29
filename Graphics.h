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

enum class FillMode {
	//埋める
	kSolid,
	//ワイヤー
	kWireFrame,
	//使用不可
	kCountOfFillMode
};

class GraphicsSystem
{

public:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	~GraphicsSystem();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	void PreDraw(ID3D12GraphicsCommandList* commandList);


	//ブレンドモード取得
	BlendMode GetBlendMode()const { return blendMode_; }

	//ブレンドモードを設定
	void SetBlendMode(BlendMode blend);

	void SetFillMode(FillMode fillMode) { fillMode_ = fillMode; }
private:
	
	
private:
	
	
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState[static_cast<int>(FillMode::kCountOfFillMode)][static_cast<int>(BlendMode::kCountOfBlendMode)] = { nullptr };

	//blendMode1
	BlendMode blendMode_ = BlendMode::kNormal;

	//fillMode
	FillMode fillMode_ = FillMode::kSolid;
};

