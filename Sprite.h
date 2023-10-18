#pragma once
#include<d3d12.h>

#include"DirectXFunc.h"
#include"struct.h"

#include<wrl.h>
using namespace Microsoft::WRL;

class Sprite {
public:

	
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

	DirectXFunc* DXF = nullptr;



	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};

	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};

	ComPtr<ID3D12Resource> transformationMatrixResourceSprite;

	WorldTransformation* transformationMatrixDataSprite = nullptr;

	Material* materialSpriteData = nullptr;

	ComPtr<ID3D12Resource> materialSpriteResource;
};