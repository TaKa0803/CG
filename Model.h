#pragma once
#include<d3d12.h>
#include"Matrix.h"
#include<string>
#include"DirectXFunc.h"

class Model {


public:
	//円作成
	static Model* CreateSphere(float kSubdivision, bool enableLighting);

	/// <summary>
	/// OBJ作成
	/// </summary>
	/// <param name="filePath">名前.obj</param>
	/// <returns></returns>
	static Model* CreateFromOBJ(const std::string& filePath);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="WVP"></param>
	/// <param name="worldMatrix"></param>
	/// <param name="texture"></param>
	void Draw(Matrix4x4 WVP, Matrix4x4 worldMatrix, int texture);



	void IsEnableTexture(bool ans) { materialData_->enableTexture = ans; }
#pragma region セッター
	void SetUV(Matrix4x4 uv) { materialData_->uvTransform = uv; }
#pragma endregion

	


	void DebugParameter();
private:
	//初期化
	void Initialize(std::string name,int point,D3D12_VERTEX_BUFFER_VIEW vertexBufferView,
		ID3D12Resource* wvpResource,
		WorldTransformation* wvpData, 
		ID3D12Resource* materialResourceS,
		Material* materialData,
		ID3D12Resource* directionalLightResource
	);

private:

	DirectXFunc* DXF;

	std::string name;

	bool isDebug = false;

	//頂点数
	int point_;

	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	ID3D12Resource* wvpResource_;
	WorldTransformation* wvpData_ = nullptr;

	ID3D12Resource* materialResource_;
	Material* materialData_ = nullptr;

	ID3D12Resource* directionalLightResource_;
};