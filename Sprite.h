#pragma once
#include<d3d12.h>

#include"DirectXFunc.h"
#include"struct.h"
#include"Vector2.h"
#include<wrl.h>



class Sprite {
public:

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXFunc* DXF_);


	void Draw(Matrix4x4 World, int texture);

	void SetUV_T(Matrix4x4 uvT){ materialSpriteData->uvTransform = uvT; }
	void IsEnableTexture(bool ans) { materialSpriteData->enableTexture = ans; }

	Material GetMaterialData() { return *materialSpriteData; }
	void SetMaterialDataColor(Vector4 color) { materialSpriteData->color = color; }
private:

	struct WorldTransformation {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};


	DirectXFunc* DXF = nullptr;



	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};

	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};

	ComPtr<ID3D12Resource> transformationMatrixResourceSprite;

	WorldTransformation* transformationMatrixDataSprite = nullptr;

	Material* materialSpriteData = nullptr;

	ComPtr<ID3D12Resource> materialSpriteResource;
};