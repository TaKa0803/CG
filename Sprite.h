#pragma once
#include<d3d12.h>

#include"DirectXFunc.h"
#include"struct.h"
#include"Vector2.h"
#include<wrl.h>
#include"Graphics.h"


class Sprite {
public:

	~Sprite();

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	
	struct WorldTransformation {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	static Sprite* Create(int texture, const Vector2& size, const Vector2& anchor = { 0.5f,0.5f });

	void Draw(int texture=-1);

	void SetPosition(Vector2 pos) { pos_.x = pos.x; pos_.y = pos.y; }
	
	void SetUV_T(Matrix4x4 uvT){ materialData_->uvTransform = uvT; }
	
	void IsEnableTexture(bool ans) { materialData_->enableTexture = ans; }

	Material GetMaterialData() { return *materialData_; }
	
	void SetMaterialDataColor(Vector4 color) { materialData_->color = color; }
private:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(int texture, ID3D12Resource* vertexResourceSprite, ID3D12Resource* indexResourceSprite, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
		D3D12_INDEX_BUFFER_VIEW& indexBufferView,
		ID3D12Resource* transformationMatrixResource,
		WorldTransformation* transformationMatrixData,
		Material* materialData,
		ID3D12Resource* materialResource);

private:


	DirectXFunc* DXF = nullptr;

	GraphicsSystem* grarphics_;


	int texture_;

	ID3D12Resource* vertexResource_;
	ID3D12Resource* indexResourceSprite_;
	//
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	//
	ID3D12Resource* transformationMatrixResource_;
	//
	WorldTransformation* transformationMatrixData_ = nullptr;

	Material* materialData_;
	//
	ID3D12Resource* materialResource_;



	Matrix4x4 viewMatrixSprite = MakeIdentity4x4();


	Vector3 pos_;
	Vector3 rotate_;
	Vector3 scale_ = {1.0f,1.0f,1.0f};
};