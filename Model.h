#pragma once
#include<d3d12.h>
#include"Matrix.h"
#include<string>
#include"DirectXFunc.h"
#include"Graphics.h"
#include"Vector2.h"

#include<wrl.h>



class Model {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Model();

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;


	/// <summary>
	/// 円の作成
	/// </summary>
	/// <param name="kSubdivision">分割量</param>
	/// <param name="enableLighting">影をつけるか</param>
	/// <returns>モデルデータ</returns>
	static Model* CreateSphere(float kSubdivision, bool enableLighting);

	/// <summary>
	/// OBJ作成
	/// </summary>
	/// <param name="filePath">名前.obj</param>
	/// <returns><モデルデータ/returns>
	static Model* CreateFromOBJ(const std::string& filePath);


public:
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="WVP"></param>
	/// <param name="viewProjection"></param>
	/// <param name="texture"></param>
	void Draw(const Matrix4x4& WVP,const Matrix4x4& viewProjection, int texture);

	/// <summary>
	/// デバッグ用ImGuiの表示
	/// </summary>
	void DebugParameter();

	/// <summary>
	/// シェーダー処理の切り替え
	/// </summary>
	/// <param name="ans">影をつけるか</param>
	void IsEnableShader(bool ans) { materialData_->enableLighting = ans; }

	/// <summary>
	/// 画像を使用するか
	/// </summary>
	/// <param name="ans">画像を使うか</param>
	void IsEnableTexture(bool ans) { materialData_->enableTexture = ans; }
#pragma region セッター
	
	/// <summary>
	/// uv座標の変換
	/// </summary>
	/// <param name="uv">uvMatrix</param>
	void SetUV(Matrix4x4 uv) { materialData_->uvTransform = uv; }

	/// <summary>
	/// 色の変更
	/// </summary>
	/// <param name="color">色</param>
	void SetColor(Vector4 color) { materialData_->color = color; }
#pragma endregion

	/// <summary>
	/// 色の取得
	/// </summary>
	/// <returns>色</returns>
	Vector4 GetColor() { return materialData_->color; }

	
private:
	struct WorldTransformation {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};
	
	//初期化
	void Initialize(std::string name,int point, ID3D12Resource* vertexRtea,D3D12_VERTEX_BUFFER_VIEW vertexBufferView,
		ID3D12Resource* wvpResource,
		WorldTransformation* wvpData, 
		ID3D12Resource* materialResourceS,
		Material* materialData,
		ID3D12Resource* directionalLightResource
	);

private:
	
	DirectXFunc* DXF_;

	std::unique_ptr<GraphicsSystem> grarphics_;


	std::string name;

	bool isDebug = false;

	//頂点数
	int point_;


	ID3D12Resource* vertexRtea_;
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	ID3D12Resource* wvpResource_;
	WorldTransformation* wvpData_ = nullptr;

	ID3D12Resource* materialResource_;
	Material* materialData_ = nullptr;

	ID3D12Resource* directionalLightResource_;
};