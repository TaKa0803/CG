#include"Sprite.h"
#include"Matrix.h"
#include<cassert>
#include"TextureManager.h"
#include"SRVManager.h"
#include"function.h"
#include<imgui.h>
#include"Log.h"


Sprite::Sprite() {
}

Sprite::~Sprite() {
	delete grarphics_;
	delete particlegraphics_;
	vertexResource_->Release();
	indexResource_->Release();
	transformationMatrixResource_->Release();
	materialResource_->Release();
}

void Sprite::DrawDebugImGui(const char* name) {

	Vector4 color = materialData_->color;


#ifdef _DEBUG
	ImGui::Begin(name);
	ImGui::DragFloat2("pos", &pos_.x, 0.1f);
	ImGui::DragFloat("rotate", &rotate_.z);
	ImGui::DragFloat2("scale", &scale_.x, 0.01f);

	ImGui::Text("UV");
	ImGui::DragFloat2("uv pos", &uvpos.x, 0.1f);
	ImGui::DragFloat("uv rotate", &uvrotate.z, 0.1f);
	ImGui::DragFloat2("uv scale", &uvscale.x, 0.1f);
	ImGui::End();
#endif // _DEBUG

}


Sprite* Sprite::Create(int texture, const Vector2& size, const Vector2& anchor) {

	DirectXFunc* DXF = DirectXFunc::GetInstance();

#pragma region Sprite
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};

	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};

	ID3D12Resource* transformationMatrixResourceSprite;

	WorldTransformation* transformationMatrixDataSprite = nullptr;

	Material* materialSpriteData = nullptr;

	ID3D12Resource* materialSpriteResource;


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

	Vector2 minv = { size.x * (-anchor.x),size.y * (-anchor.y) };

	Vector3 maxV = { size.x * (1 - anchor.x),size.y * (1 - anchor.y) };

	vertexDataSprite[0].position = { minv.x,maxV.y,0.0f,1.0f };
	vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite[0].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[1].position = { minv.x,minv.y,0.0f,1.0f };
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite[1].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[2].position = { maxV.x,maxV.y,0.0f,1.0f };
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	vertexDataSprite[2].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[3].position = { maxV.x,minv.y,0.0f,1.0f };
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


	Sprite* sprite = new Sprite();
	sprite->Initialize(texture, vertexResourceSprite, indexResourceSprite, vertexBufferViewSprite, indexBufferViewSprite, transformationMatrixResourceSprite, transformationMatrixDataSprite, materialSpriteData, materialSpriteResource);

	return sprite;

}

Sprite* Sprite::CreateInstancing(int texture, const Vector2& size, const int num, const Vector2& anchor) {
	DirectXFunc* DXF = DirectXFunc::GetInstance();

#pragma region Sprite
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	ID3D12Resource* transformationMatrixResource;

	WorldTransformation* instancingData = nullptr;

	Material* materialData = nullptr;

	ID3D12Resource* materialResource;


#pragma region VertexResourceとVertexBufferViewを用意
	//Sprite用の頂点リソースを作る
	ID3D12Resource* vertexResourceSprite = CreateBufferResource(DXF->GetDevice(), sizeof(VertexData) * 4);

	//頂点バッファビューを作成する

	//リソース用の先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
	//頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);
#pragma endregion

#pragma region 頂点データを設定する
	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	//一枚目の三角形

	Vector2 minv = { size.x * (-anchor.x),size.y * (-anchor.y) };

	Vector3 maxV = { size.x * (1 - anchor.x),size.y * (1 - anchor.y) };

	vertexDataSprite[0].position = { minv.x,maxV.y,0.0f,1.0f };
	vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite[0].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[1].position = { minv.x,minv.y,0.0f,1.0f };
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite[1].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[2].position = { maxV.x,maxV.y,0.0f,1.0f };
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	vertexDataSprite[2].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[3].position = { maxV.x,minv.y,0.0f,1.0f };
	vertexDataSprite[3].texcoord = { 1.0f,0.0f };
	vertexDataSprite[3].normal = { 0.0f,0.0f,-1.0f };

	ID3D12Resource* indexResourceSprite = CreateBufferResource(DXF->GetDevice(), sizeof(uint32_t) * 6);

	//リソースの先頭アドレスから使う
	indexBufferView.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス６つ分のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックス
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

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
	transformationMatrixResource = CreateBufferResource(DXF->GetDevice(), sizeof(WorldTransformation) * num);
	//データを書き込む

	//書き込むためのアドレスを取得
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));

	for (uint32_t index = 0; index < (uint32_t)num; ++index) {
		instancingData[index].WVP = MakeIdentity4x4();
		instancingData[index].World = MakeIdentity4x4();
	}

#pragma endregion

	//Sprite用のマテリアルリソース
	materialResource = CreateBufferResource(DXF->GetDevice(), sizeof(Material));
	//マテリアルにデータを書き込む

	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = false;
	materialData->uvTransform = MakeIdentity4x4();
	materialData->enableTexture = true;
	materialData->enableHalfLambert = false;
#pragma endregion


	D3D12_SHADER_RESOURCE_VIEW_DESC instancingDesc{};
	instancingDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingDesc.Buffer.FirstElement = 0;
	instancingDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingDesc.Buffer.NumElements = num;
	instancingDesc.Buffer.StructureByteStride = sizeof(WorldTransformation);

	SRVManager* SRVM = SRVManager::GetInstance();
	int insSRVHandle = SRVM->CreateSRV(transformationMatrixResource, nullptr, instancingDesc);

	Sprite* sprite = new Sprite();
	sprite->Initialize(texture, vertexResourceSprite, indexResourceSprite, vertexBufferView, indexBufferView, transformationMatrixResource, instancingData, materialData, materialResource, insSRVHandle, num);

	return sprite;

}

int Sprite::isntancingCount_ = 0;

void Sprite::Initialize(int texture,
	ID3D12Resource* vertexResourceSprite,
	ID3D12Resource* indexResourceSprite,
	D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
	D3D12_INDEX_BUFFER_VIEW& indexBufferView,
	ID3D12Resource* transformationMatrixResource,
	WorldTransformation* transformationMatrixData,
	Material* materialData,
	ID3D12Resource* materialResource,
	int instancingHandle, int instancingCount
) {
	DXF = DirectXFunc::GetInstance();



	//データコピー

	texture_ = texture;
	vertexResource_ = vertexResourceSprite;
	indexResource_ = indexResourceSprite;
	vertexBufferView_ = vertexBufferView;
	indexBufferView_ = indexBufferView;
	transformationMatrixData_ = transformationMatrixData;
	transformationMatrixResource_ = transformationMatrixResource;
	materialData_ = materialData;
	materialResource_ = materialResource;

	//インスタンシング初期化の場合の処理
	if (instancingHandle != -1) {
		instancingHandleNum = instancingHandle;
		isntancingCount_ = instancingCount;
		particlegraphics_ = new ParticleGraphics();
		particlegraphics_->Initialize(DXF->GetDevice());
	}
	else {
		grarphics_ = new GraphicsSystem();
		grarphics_->Initialize(DXF->GetDevice());

	}

	Log("Sprite is Created!\n");
}

void Sprite::Draw(int texture) {

	grarphics_->PreDraw(DXF->GetCMDList());

	//uvTransform更新
	materialData_->uvTransform = MakeAffineMatrix(uvscale, uvrotate, uvpos);

	//ワールド更新
	Matrix4x4 World = MakeAffineMatrix(scale_, rotate_, pos_);


	//スプライト用データ
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WindowApp::kClientWidth), float(WindowApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 VPSprite = viewMatrixSprite * projectionMatrixSprite;
	Matrix4x4 WVP = World * VPSprite;
	//データ代入
	transformationMatrixData_->WVP = WVP;
	transformationMatrixData_->World = World;

	//Spriteの描画
	DXF->GetCMDList()->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定			
	DXF->GetCMDList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
	DXF->GetCMDList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルCBufferの場所を設定
	DXF->GetCMDList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	DXF->GetCMDList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	//
	if (texture != -1) {
		DXF->GetCMDList()->SetGraphicsRootDescriptorTable(2, SRVManager::GetInstance()->GetTextureDescriptorHandle(texture));
	}
	else {
		DXF->GetCMDList()->SetGraphicsRootDescriptorTable(2, SRVManager::GetInstance()->GetTextureDescriptorHandle(texture_));
	}
	//描画！！（DrawCall
	DXF->GetCMDList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::DrawInstancing(int texture, const Vector3 pos[]) {
	
	particlegraphics_->PreDraw(DXF->GetCMDList());
	

	//uvTransform更新
	materialData_->uvTransform = MakeAffineMatrix(uvscale, uvrotate, uvpos);


	for (uint32_t index = 0;index < (uint32_t)isntancingCount_; ++index) {
		//ワールド更新
		Matrix4x4 World = MakeAffineMatrix(scale_, rotate_, pos[index]+pos_);


		//スプライト用データ
		Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WindowApp::kClientWidth), float(WindowApp::kClientHeight), 0.0f, 100.0f);
		Matrix4x4 VPSprite = viewMatrixSprite * projectionMatrixSprite;
		Matrix4x4 WVP = World * VPSprite;
		//データ代入
		transformationMatrixData_[index].WVP = WVP;
		transformationMatrixData_[index].World = World;
	}
	//Spriteの描画
	DXF->GetCMDList()->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定			
	DXF->GetCMDList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
	DXF->GetCMDList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルCBufferの場所を設定
	DXF->GetCMDList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	//DXF->GetCMDList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	
	//インスタンシング座標設定
	DXF->GetCMDList()->SetGraphicsRootDescriptorTable(1, SRVManager::GetInstance()->GetTextureDescriptorHandle(instancingHandleNum));
	//
	if (texture != -1) {
		DXF->GetCMDList()->SetGraphicsRootDescriptorTable(2, SRVManager::GetInstance()->GetTextureDescriptorHandle(texture));
	}
	else {
		DXF->GetCMDList()->SetGraphicsRootDescriptorTable(2, SRVManager::GetInstance()->GetTextureDescriptorHandle(texture_));
	}
	//描画！！（DrawCall
	DXF->GetCMDList()->DrawIndexedInstanced(6, isntancingCount_, 0, 0, 0);

}


