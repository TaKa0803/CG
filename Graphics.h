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

//ルートシグネチャマネージャー
class RootSignatureManager {

public://シングルトンパターン
	static RootSignatureManager* GetInstance();


private:
	RootSignatureManager() = default;
	~RootSignatureManager() = default;
	RootSignatureManager(const RootSignatureManager& o) = delete;
	const RootSignatureManager& operator=(const RootSignatureManager& o) = delete;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device"></param>
	void Initialize(ID3D12Device*device);

	/// <summary>
	/// ３D用のRootSignature取得
	/// </summary>
	/// <returns></returns>
	std::vector<std::vector<ID3D12PipelineState*>>Get3DRootSignature() { return rootSignature3D_; }

	/// <summary>
	/// ２D用のRootSignature取得
	/// </summary>
	/// <returns></returns>
	std::vector<ID3D12PipelineState*>Get2DRootSignature() { return rootSignature2D_; }

private:

	void Load3DRootsignature(ID3D12Device* device);

	void Load2DRootsignature(ID3D12Device* device);


	//3D用ルートシグネチャ
	std::vector<std::vector<ID3D12PipelineState*>>rootSignature3D_;

	//2D用ルートシグネチャ
	std::vector<ID3D12PipelineState*>rootSignature2D_;
};


class GraphicsSystem
{

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
	ID3D12PipelineState* graphicsPipelineState[static_cast<int>(FillMode::kCountOfFillMode)][static_cast<int>(BlendMode::kCountOfBlendMode)] = { nullptr };

	//blendMode1
	BlendMode blendMode_ = BlendMode::kNormal;

	//fillMode
	FillMode fillMode_ = FillMode::kSolid;
};


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
