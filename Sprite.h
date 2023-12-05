#pragma once
#include<d3d12.h>

#include"DirectXFunc.h"
#include"struct.h"
#include"Vector2.h"
#include<wrl.h>
#include"Graphics.h"


class Sprite {
public:

	Sprite();

	~Sprite();

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	
	/// <summary>
	/// スプライト作成
	/// </summary>
	/// <param name="texture">画像の要素番号</param>
	/// <param name="size">画像サイズ</param>
	/// <param name="anchor">アンカー</param>
	/// <returns>データ返却</returns>
	static Sprite* Create(int texture, const Vector2& size, const Vector2& anchor = { 0.5f,0.5f });

	static Sprite* CreateInstancing(int texture, const Vector2& size, const int num, const Vector2& anchor = { 0.5f,0.5f });

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

	void DrawInstancing(int texture = -1, const Vector3 pos[]=nullptr);
public:///セッター

	/// <summary>
	/// 座標代入
	/// </summary>
	/// <param name="pos">代入座標</param>
	void SetPosition(const Vector2 pos) { pos_.x = pos.x; pos_.y = pos.y; }
	
	
	/// <summary>
	/// uvの平行移動代入
	/// </summary>
	/// <param name="uvPos">代入する</param>
	void SetTVTranslate(const Vector2 uvPos) { uvpos.x = uvPos.x; uvpos.y = uvPos.y; }

	

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
	
	/// <summary>
	/// 画像のカラーを設定
	/// </summary>
	/// <param name="color"></param>
	void SetMaterialDataColor(const Vector4& color) { materialData_->color = color; }
private:

	struct WorldTransformation {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(int texture, ID3D12Resource* vertexResourceSprite, ID3D12Resource* indexResourceSprite, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
		D3D12_INDEX_BUFFER_VIEW& indexBufferView,
		ID3D12Resource* transformationMatrixResource,
		WorldTransformation* transformationMatrixData,
		Material* materialData,
		ID3D12Resource* materialResource, int instancingHandle=-1,int instancingCount=0);

private:


	DirectXFunc* DXF = nullptr;

	GraphicsSystem* grarphics_=nullptr;

	ParticleGraphics* particlegraphics_ = nullptr;

	int texture_=-1;

	int instancingHandleNum = -1;

	static int isntancingCount_;

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

	Material* materialData_ = nullptr;;
	//
	ID3D12Resource* materialResource_=nullptr;



	Matrix4x4 viewMatrixSprite = MakeIdentity4x4();


	Vector3 pos_;
	Vector3 rotate_;
	Vector3 scale_ = {1.0f,1.0f,1.0f};


	Vector3 uvpos{};
	Vector3 uvscale{ 1.0f,1.0f,1.0f };
	Vector3 uvrotate{};
};