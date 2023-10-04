#pragma once
#include<d3d12.h>

#include"DirectXFunc.h"
#include"struct.h"

class Sprite {
public:

	static ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXFunc* DXF_);


	void Draw(Matrix4x4 WVP,Matrix4x4 World, D3D12_GPU_DESCRIPTOR_HANDLE texture);

	void SetUV_T(Matrix4x4 uvT){ materialSpriteData->uvTransform = uvT; }
	void IsEnableTexture(bool ans) { materialSpriteData->enableTexture = ans; }

	Material GetMaterialData() { return *materialSpriteData; }
	void SetMaterialDataColor(Vector4 color) { materialSpriteData->color = color; }
private:

	DirectXFunc* DXF = nullptr;



	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};

	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};

	ID3D12Resource* transformationMatrixResourceSprite;

	WorldTransformation* transformationMatrixDataSprite = nullptr;

	Material* materialSpriteData = nullptr;

	ID3D12Resource* materialSpriteResource;
};