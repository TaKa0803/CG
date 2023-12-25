#pragma once
#include<d3d12.h>

#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"
#include"Math/Vector2.h"
#include<wrl.h>
#include"Graphics/Graphics.h"

#include"ParticleEmiter/ParticleEmiter.h"

#include"WorldTransform/WorldTransform.h"

class Sprite {
public:

	Sprite();

	~Sprite();

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	
	/// <summary>
	/// スプライト作成
	/// </summary>
	/// <param name="texture">画像の要素番号</param>
	/// <param name="anchor">アンカー</param>
	/// <returns>データ返却</returns>
	static Sprite* Create(int texture,const Vector2 translate={640,360}, const float rotate=0, const Vector2 scale={1,1}, const Vector2 anchor = {0.5f,0.5f});

	/// <summary>
	/// スプライト作成
	/// </summary>
	/// <param name="texture">画像の要素番号</param>
	/// <param name="size">画像サイズ</param>
	/// <param name="Rect">画像のどのサイズ描画するか</param>
	/// <param name="anchor">アンカー</param>
	/// <returns>データ返却</returns>
	static Sprite* Create(int texture, const Vector2 size,const Vector2 Rect, const Vector2 translate = { 640,360 }, const float rotate = 0, const Vector2 scale = { 1,1 }, const Vector2 anchor = { 0.5f,0.5f });

	/// <summary>
	/// デバッグウィンドウ表示
	/// </summary>
	/// <param name="name">ウィンドウの名前</param>
	void DrawDebugImGui(const char* name);

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="texture">画像の要素番号（無くてもいい）</param>
	void Draw(int texture=-1);

public:///セッター

	/// <summary>
	/// 座標代入
	/// </summary>
	/// <param name="pos">代入座標</param>
	void SetPosition(const Vector3& pos) { world_.translate_ = pos; }
	
	/// <summary>
	/// 回転量代入
	/// </summary>
	/// <param name="rotate"></param>
	void SetRotate(const float rotate) { world_.rotate_.z = rotate; }

	/// <summary>
	/// サイズ設定
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(const Vector3& scale) { world_.scale_ = scale; }

	/// <summary>
	/// サイズ設定（等倍
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(const float scale) { world_.scale_ = { scale,scale,1 }; }

	/// <summary>
	/// uvの平行移動代入
	/// </summary>
	/// <param name="uvPos">代入する</param>
	void SetTVTranslate(const Vector2 uvPos) { uvpos.x = uvPos.x; uvpos.y = uvPos.y; }


	/// <summary>
	/// 画像のカラーを設定
	/// </summary>
	/// <param name="color"></param>
	void SetMaterialDataColor(const Vector4& color) { materialData_->color = color; }

	/// <summary>
	/// 画像を使うか否か
	/// </summary>
	/// <param name="ans">正否</param>
	void IsEnableTexture(const bool ans) { materialData_->enableTexture = ans; }

	/// <summary>
	/// マテリアルデータ構造体取得
	/// </summary>
	/// <returns>マテリアルデータ</returns>
	Material GetMaterialData() { return *materialData_; }
	
	Vector3 GetScale()const { return world_.scale_; }


private:

	struct WorldTransformation {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(int texture,

	WorldTransform world,

	ID3D12Resource* vertexResource,

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView,

	ID3D12Resource* indexResourceSprite,

	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite
	);

	
private:


	DirectXFunc* DXF = nullptr;

	GraphicsSystem* grarphics_=nullptr;


	int texture_=-1;

	
	
	ID3D12Resource* vertexResource_=nullptr;
	ID3D12Resource* indexResource_=nullptr;
	//
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	//
	ID3D12Resource* transformationMatrixResource_ = nullptr;;
	//
	WorldTransformation* transformationMatrixData_ = nullptr;

	Particle4GPU* particle4GPUData_ = nullptr;

	Material* materialData_ = nullptr;;
	//
	ID3D12Resource* materialResource_=nullptr;



	Matrix4x4 viewMatrixSprite = MakeIdentity4x4();

	

	WorldTransform world_;

	Vector3 uvpos{};
	Vector3 uvscale{ 1.0f,1.0f,1.0f };
	Vector3 uvrotate{};
};