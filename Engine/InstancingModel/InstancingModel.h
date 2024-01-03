#pragma once

#include<wrl.h>
#include<string>

#include"DirectXFunc/DirectXFunc.h"
#include"InstancingPSO/InstancingPSO.h"
#include"Math/Vector2.h"
#include"Math/Matrix.h"
#include"struct.h"
#include"ModelManager/ModelManager.h"



class InstancingModel {
public:


	/// <summary>
	/// デストラクタ
	/// </summary>
	~InstancingModel();

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// OBJ作成
	/// </summary>
	/// <param name="filePath">resources以降のフォルダ</param>
	/// <returns><モデルデータ/returns>
	static InstancingModel* CreateFromOBJ(const std::string& directory, const std::string& filePath,int instancingNum);

	static InstancingModel* CreateFromGLTF(const std::string& directory, const std::string& filePath, int instancingNum);

public:

	/// <summary>
	/// 更新前処理
	/// </summary>
	void PreUpdate();

	/// <summary>
	/// ワールドを追加
	/// </summary>
	/// <param name="world"></param>
	void AddWorld(const WorldTransform& world);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="WVP"></param>
	/// <param name="viewProjection"></param>
	/// <param name="texture"></param>
	void Draw(const Matrix4x4& viewProjection, int texture = -1);

	/// <summary>
	/// Debug用ImGui表示
	/// </summary>
	/// <param name="name"></param>
	void DebugParameter(const char* name);

	/// <summary>
	/// シェーダー処理の切り替え
	/// </summary>
	/// <param name="ans">影をつけるか</param>
	//void IsEnableShader(bool ans) { materialData_->enableLighting = ans; }

	/// <summary>
	/// 画像を使用するか
	/// </summary>
	/// <param name="ans">画像を使うか</param>
	//void IsEnableTexture(bool ans) { materialData_->enableTexture = ans; }
#pragma region セッター

	/// <summary>
	/// uv座標の変換
	/// </summary>
	/// <param name="uv">uvMatrix</param>
	void SetUV(Matrix4x4 uv) { materialData_->uvTransform = uv; }

	void SetUVTranslate(Vector2 pos) { uvWorld_.translate_.x = pos.x; uvWorld_.translate_.y = pos.y; }

	void SetUVScale(Vector3 scale) { uvWorld_.scale_ = scale; }

	/// <summary>
	/// 色の変更
	/// </summary>
	/// <param name="color">色</param>
	void SetColor(Vector4 color) { materialData_->color = color; }

	void SetAlpha(float alpha) { materialData_->color.w = alpha; }

	void SetBlendMode(BlendMode blend) { pso_->SetBlendMode(blend); }

	void SetFillMode(FillMode fillmode) { pso_->SetFillMode(fillmode); }

	void SetTag(const std::string& tag) { tag_ = tag; }
#pragma endregion

	/// <summary>
	/// 色の取得
	/// </summary>
	/// <returns>色</returns>
	const Vector4 GetColor() { return materialData_->color; }

	const Material* GetMaterialData() { return materialData_; }

	const Vector3 GetUVScale()const { return uvWorld_.scale_; }

	const std::string GetTag() { return tag_; }
private:


	//初期化
	void Initialize(
		ModelData modelData,
		int point,
		int instancingNum,
		ID3D12Resource* vertexRtea,
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView,
		bool isOBJ

	);

private:

	//
	DirectXFunc* DXF_;

	InstancingPSO* pso_;


	std::string tag_;

	D3D12_GPU_DESCRIPTOR_HANDLE texture_;

	D3D12_GPU_DESCRIPTOR_HANDLE instancingHandle_;

	int instancingNum_;

	bool isDebug = false;

	bool isOBJ_ = true;

	//頂点数
	int point_;

	//モデルデータ
	ModelData modelData_;

	std::vector<AnimationVertex>animationVertexes_;

	ID3D12Resource* vertexData_;
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	

	ID3D12Resource* wvpResource_;
	WorldTransformation* wvpData_ = nullptr;

	ID3D12Resource* materialResource_;
	Material* materialData_ = nullptr;

	ID3D12Resource* directionalLightResource_;
	DirectionalLight* directionalLightData_ = nullptr;

	//UVのワールド
	WorldTransform uvWorld_;

	//ワールド軍
	std::vector<std::unique_ptr<WorldTransform>>worlds_;

};
