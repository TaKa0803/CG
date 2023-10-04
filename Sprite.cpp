#include"Sprite.h"
#include"Matrix.h"
#include<cassert>

ID3D12Resource* Sprite::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {

#pragma region VertexResourceを生成する
	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResorceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResorceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResorceDesc.Width = sizeInBytes;
	//バッファの場合はこれらは１にする決まり
	vertexResorceDesc.Height = 1;
	vertexResorceDesc.DepthOrArraySize = 1;
	vertexResorceDesc.MipLevels = 1;
	vertexResorceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	vertexResorceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
#pragma endregion
	ID3D12Resource* vertexResource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResorceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));
	return vertexResource;
}


void Sprite::Initialize(DirectXFunc* DXF_)
{
	assert(DXF_);
	DXF = DXF_;

#pragma region Sprite
#pragma region VertexResourceとVertexBufferViewを用意
	//Sprite用の頂点リソースを作る
	ID3D12Resource* vertexResourceSprite = CreateBufferResource(DXF->GetDevice(), sizeof(VertexData) * 4);

	//頂点バッファビューを作成する
	
	//リソース用の先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 4;
	//頂点当たりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);
#pragma endregion
#pragma region 頂点データを設定する
	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	//一枚目の三角形
	vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite[0].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite[1].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	vertexDataSprite[2].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[3].position = { 640.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[3].texcoord = { 1.0f,0.0f };
	vertexDataSprite[3].normal = { 0.0f,0.0f,-1.0f };

	ID3D12Resource* indexResourceSprite = CreateBufferResource(DXF->GetDevice(), sizeof(uint32_t) * 6);
	
	//リソースの先頭アドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス６つ分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックス
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 1;
	indexDataSprite[1] = 3;
	indexDataSprite[2] = 0;

	indexDataSprite[3] = 3;
	indexDataSprite[4] = 2;
	indexDataSprite[5] = 0;


#pragma endregion
#pragma region Transform周りを作る
	transformationMatrixResourceSprite = CreateBufferResource(DXF->GetDevice(), sizeof(WorldTransformation));
	//データを書き込む
	
	//書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	transformationMatrixDataSprite->WVP = MakeIdentity4x4();
	transformationMatrixDataSprite->World = MakeIdentity4x4();
#pragma endregion

	//Sprite用のマテリアルリソース
	materialSpriteResource = CreateBufferResource(DXF->GetDevice(), sizeof(Material));
	//マテリアルにデータを書き込む
	
	materialSpriteResource->Map(0, nullptr, reinterpret_cast<void**>(&materialSpriteData));
	materialSpriteData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialSpriteData->enableLighting = false;
	materialSpriteData->uvTransform = MakeIdentity4x4();
	materialSpriteData->enableTexture = true;
	materialSpriteData->enableHalfLambert = false;
#pragma endregion
}

void Sprite::Draw(Matrix4x4 WVP, Matrix4x4 World, D3D12_GPU_DESCRIPTOR_HANDLE texture)
{

	transformationMatrixDataSprite->WVP = WVP;
	transformationMatrixDataSprite->World = World;

	//Spriteの描画
	DXF->GetCMDList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);	//VBVを設定			
	DXF->GetCMDList()->IASetIndexBuffer(&indexBufferViewSprite);//IBVを設定
	//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
	DXF->GetCMDList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルCBufferの場所を設定
	DXF->GetCMDList()->SetGraphicsRootConstantBufferView(0, materialSpriteResource->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	DXF->GetCMDList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
	//
	DXF->GetCMDList()->SetGraphicsRootDescriptorTable(2, texture);
	//描画！！（DrawCall
	DXF->GetCMDList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
