#pragma once
#include<d3d12.h>

#include"DirectXFunc.h"

class Sprite {
public:

	static ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXFunc* DXF_);


	void Draw();
private:

	DirectXFunc* DXF = nullptr;
};