#pragma once

#include<string>
#include<d3d12.h>
#include<dxcapi.h>
#include<wrl.h>
#include<vector>


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

//塗りつぶし設定
enum class FillMode {
	//埋める
	kSolid,
	//ワイヤー
	kWireFrame,
	//使用不可
	kCountOfFillMode
};

class GraphicsSystem {

public:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	GraphicsSystem();
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
	ID3D12PipelineState* graphicsPipelineState[int(FillMode::kCountOfFillMode)][int(BlendMode::kCountOfBlendMode)] = { nullptr };

	//blendMode1
	BlendMode blendMode_ = BlendMode::kNormal;

	//fillMode
	FillMode fillMode_ = FillMode::kSolid;
};


#pragma region インスタンスんぐ用GPL

class InstancingGraphicsSystem {

public:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	InstancingGraphicsSystem();
	~InstancingGraphicsSystem();

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
	ID3D12PipelineState* graphicsPipelineState[int(FillMode::kCountOfFillMode)][int(BlendMode::kCountOfBlendMode)] = { nullptr };

	//blendMode1
	BlendMode blendMode_ = BlendMode::kNormal;

	//fillMode
	FillMode fillMode_ = FillMode::kSolid;
};

#pragma endregion



class ParticleGraphics {

public:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	ParticleGraphics();
	~ParticleGraphics();

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

private:


private:


	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState[static_cast<int>(BlendMode::kCountOfBlendMode)] = { nullptr };

	//blendMode1
	BlendMode blendMode_ = BlendMode::kNormal;


};
