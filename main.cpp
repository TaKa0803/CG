#include"Log.h"
#include"Matrix.h"
#include"struct.h"

#include"DirectXFunc.h"
#include"WinApp.h"
#include"ImGuiManager.h"
#include"Sprite.h"
#include"Graphics.h"
#include"TextureManager.h"

#include<dxgidebug.h>

#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>


#include"Matrix.h"

#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"
#include<vector>
#include<fstream>
#include<sstream>
#include<wrl.h>

#pragma region ImGui
#include"externals/imgui/imgui.h"
#pragma endregion


#define _USE_MATH_DEFINES
#include<math.h>

#pragma region 構造体
/*
struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

struct Vector2 {
	float x;
	float y;
};

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	int32_t enableTexture;
	float padding2[3];
	int32_t enableHalfLambert;
};

struct WorldTransformation {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};
*/
struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {
		//リソースリークチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1>debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			//警告時に止まる
		}
	}
};

#pragma endregion

#pragma region 関数

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	//1中で必要になる変数の宣言
	MaterialData materialdata;
	std::string line;
	//２ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());
	//３実際にファイルを読みまてりあｌDataを構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialdata.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	//４MaterialDataを返す
	return materialdata;

}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
#pragma region 中で必要となる変数の宣言
	ModelData modeldata;//構築するModelData
	std::vector<Vector4> positions;//位置
	std::vector<Vector3>normals;//法線
	std::vector<Vector2>texcoords;//texture座標
	std::string line;//ファイルからよんだ一行を格納するもの	
#pragma endregion
#pragma region ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());//開けなかったら止める
#pragma endregion
#pragma region 実際にファイルを読みModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;//先頭の識別子を読む

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			position.x *= -1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			//texcoord.x = 1.0f - texcoord.x;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;

			normals.push_back(normal);
		}
		else if (identifier == "f") {
			//面は三角形限定その他は未対応
			VertexData triangle[3];
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは位置//UV/法線で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];

				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');//区切りでインデクスを読んでいく
					elementIndices[element] = std::stoi(index);


				}
				//要素へのIndexから、実際の要素の値を取得して頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				//VertexData vertex = { position,texcoord,normal };
				//modeldata.vertices.push_back(vertex);

				triangle[faceVertex] = { position,texcoord,normal };
			}
			modeldata.vertices.push_back(triangle[2]);
			modeldata.vertices.push_back(triangle[1]);
			modeldata.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を変更する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一改装にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modeldata.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}


	}
#pragma endregion
#pragma region 4.ModelDataを返す
	return modeldata;
#pragma endregion
}


/*
ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {

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
*/
/*
//Textureデータを読む
DirectX::ScratchImage LoadTexture(const std::string& filePath) {
	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//みっぷマップ作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//みっぷマップ月のデータを返す
	return mipImages;
}

//DirectX12のTextureResourceを作る
ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
#pragma region 1.metadataをもとにResourceの設定
	//metadataをもとにResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);									//Textureの幅
	resourceDesc.Height = UINT(metadata.height);								//Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);						//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);					//奥行き　or 配列Textureの配列数
	resourceDesc.Format = metadata.format;										//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;											//サンプリングカウント１固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);		//Textureの次元数。普段使っているのは二次元
#pragma endregion
#pragma region 2.利用するHeapの設定
	//利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//細かい設定を行う

#pragma endregion
#pragma region 3.Resourceを生成する
	//Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,							//Heapの設定
		D3D12_HEAP_FLAG_NONE,						//Heapの特殊な設定。特になし。
		&resourceDesc,								//Reosurceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,			//初回のResourceState。Textureは基本読むだけ
		nullptr,									//Clear最適地。使わないのでnullptr
		IID_PPV_ARGS(&resource));					//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

#pragma endregion
	return resource;
}

//TextureResourceにデータを転送する
void UploadTextureDataa(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
	//Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		//MipLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		//Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,					//全領域へコピー
			img->pixels,				//元データアドレス
			UINT(img->rowPitch),		//１ラインサイズ
			UINT(img->slicePitch)		//１枚サイズ
		);
		assert(SUCCEEDED(hr));
	}
}
*/

/*
[[nodiscard]]
ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages,
	ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ID3D12Resource* intermediateResource = CreateBufferResource(device, intermediateSize);
	UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());
	//
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;
}
*/
/*
D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}
*/
#pragma endregion


//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	CoInitializeEx(0, COINIT_MULTITHREADED);
	D3DResourceLeakChecker lackCheck;

#pragma region 基板初期化
	WinApp* winApp = WinApp::GetInstance();
	winApp->Initialize();

	DirectXFunc* DXF = DirectXFunc::GetInstance();
	DXF->Initialize(winApp);

	GraphicsSystem *graphics = nullptr;
	graphics = new GraphicsSystem;
	graphics->Initialize(DXF->GetDevice());
#pragma endregion

#pragma region Sprite
	Sprite *sprite = new Sprite();
	sprite->Initialize(DXF);
#pragma endregion

	


/*
#pragma region 円
#pragma region VertexBufferViewを作成

	const float kSubdivision = 128;//分割数
	//頂点数
	int point = (int)kSubdivision * (int)kSubdivision * 6;
	ID3D12Resource* vertexResourceSphere = CreateBufferResource(DXF->GetDevice(), sizeof(VertexData) * point);


	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSphere{};
	//リソースの戦闘のアドレスから使う
	vertexBufferViewSphere.BufferLocation = vertexResourceSphere->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferViewSphere.SizeInBytes = sizeof(VertexData) * point;
	//1頂点当たりのサイズ
	vertexBufferViewSphere.StrideInBytes = sizeof(VertexData);
#pragma endregion
#pragma region Resourceにデータを書き込む
	//時計周りに点を設定していく
	VertexData* vertexDataS = nullptr;
	//書き込むためのアドレスを取得
	vertexResourceSphere->Map(0, nullptr,
		reinterpret_cast<void**>(&vertexDataS));
	
	//経度一つ分の角度δ
	const float kLonEvery = (float)M_PI * 2.0f / float(kSubdivision);
	//緯度一つ分の角度Θ
	const float kLatEvery = (float)M_PI / float(kSubdivision);
	//緯度の方向に分割
	for (int latIndex = 0; latIndex < kSubdivision; ++latIndex) {//σ
		float lat = -(float)M_PI / 2.0f + kLatEvery * latIndex;
		//経度のほう
		for (int lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {//Θ
			uint32_t start = ((int)latIndex * (int)kSubdivision + (int)lonIndex) * 6;
			float lon = lonIndex * kLonEvery;
			//一枚目
			//1(a)
			vertexDataS[start].position = { cos(lat) * cos(lon),sin(lat),  cos(lat) * sin(lon),1.0f };
			vertexDataS[start].texcoord ={ (lonIndex) / (float)kSubdivision,1.0f - (latIndex) / (float)kSubdivision };
			vertexDataS[start].normal = { vertexDataS[start].position.x,vertexDataS[start].position.y,vertexDataS[start].position.z };

			//2(b)
			vertexDataS[start+1].position = { cos(lat + kLatEvery) * cos(lon),sin(lat+kLatEvery), cos(lat + kLatEvery) * sin(lon),1.0f };
			vertexDataS[start+1].texcoord = { (lonIndex) / (float)kSubdivision,1.0f - (latIndex+1) / (float)kSubdivision };
			vertexDataS[start+1].normal = { vertexDataS[start+1].position.x,vertexDataS[start+1].position.y,vertexDataS[start+1].position.z };

			//3(c)
			vertexDataS[start+2].position = { cos(lat) * cos(lon + kLonEvery),sin(lat),  cos(lat) * sin(lon + kLonEvery),1.0f };
			vertexDataS[start+2].texcoord = { (lonIndex+1) / (float)kSubdivision,1.0f - (latIndex) / (float)kSubdivision };
			vertexDataS[start+2].normal = { vertexDataS[start+2].position.x,vertexDataS[start+2].position.y,vertexDataS[start+2].position.z };

			//二枚目
			//
			vertexDataS[start + 3] = vertexDataS[start + 1];
			//
			vertexDataS[start + 4].position = { cos(lat + kLatEvery) * cos(lon+kLonEvery),sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon + kLonEvery),1.0f };
			vertexDataS[start + 4].texcoord = { (lonIndex+1) / (float)kSubdivision,1.0f - (latIndex+1) / (float)kSubdivision };
			vertexDataS[start+4].normal = { vertexDataS[start+4].position.x,vertexDataS[start+4].position.y,vertexDataS[start+4].position.z };
			//
			vertexDataS[start + 5] = vertexDataS[start + 2];
		}
	}
#pragma endregion
	//WVP用のリソースを作る。Matrix４ｘ４1つ分のサイズを用意する
	ID3D12Resource* wvpResourceS = CreateBufferResource(DXF->GetDevice(), sizeof(WorldTransformation));
	//データを書き込む
	WorldTransformation* wvpDataS = nullptr;
	//書き込むためのアドレスを取得
	wvpResourceS->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataS));
	//単位行列を書き込んでおくtextureResource
	wvpDataS->WVP = MakeIdentity4x4();
	wvpDataS->World = MakeIdentity4x4();
#pragma endregion	
#pragma region obj
	ModelData modeldata = LoadObjFile("resources","bunny.obj");
	//頂点リソースを作る
	ID3D12Resource* vertexResource = CreateBufferResource(DXF->GetDevice(), sizeof(VertexData) * modeldata.vertices.size());
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modeldata.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modeldata.vertices.data(), sizeof(VertexData)* modeldata.vertices.size());
#pragma endregion
#pragma region Utah Teapot
	ModelData modeltea = LoadObjFile("resources", "teapot.obj");
	ID3D12Resource* vertexRtea = CreateBufferResource(DXF->GetDevice(), sizeof(VertexData) * modeltea.vertices.size());
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewtea{};
	vertexBufferViewtea.BufferLocation = vertexRtea->GetGPUVirtualAddress();
	vertexBufferViewtea.SizeInBytes = UINT(sizeof(VertexData) * modeltea.vertices.size());
	vertexBufferViewtea.StrideInBytes = sizeof(VertexData);

	VertexData* vertexDatatea = nullptr;
	vertexRtea->Map(0, nullptr, reinterpret_cast<void**>(&vertexDatatea));
	std::memcpy(vertexDatatea, modeltea.vertices.data(), sizeof(VertexData) * modeltea.vertices.size());


	//WVP用のリソースを作る。Matrix４ｘ４1つ分のサイズを用意する
	ID3D12Resource* wvpResourceTea = CreateBufferResource(DXF->GetDevice(), sizeof(WorldTransformation));
	//データを書き込む
	WorldTransformation* wvpDataTea = nullptr;
	//書き込むためのアドレスを取得
	wvpResourceTea->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataTea));
	//単位行列を書き込んでおくtextureResource
	wvpDataTea->WVP = MakeIdentity4x4();
	wvpDataTea->World = MakeIdentity4x4();
#pragma endregion
#pragma region 三角形
#pragma region vertex
	int pointT = 6;
	ID3D12Resource* vertexResourceTri = CreateBufferResource(DXF->GetDevice(), sizeof(VertexData) * pointT);
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewTri{};
	//リソースの戦闘のアドレスから使う
	vertexBufferViewTri.BufferLocation = vertexResourceTri->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferViewTri.SizeInBytes = sizeof(VertexData) * pointT;
	//1頂点当たりのサイズ
	vertexBufferViewTri.StrideInBytes = sizeof(VertexData);
#pragma endregion
#pragma region data
	//時計周りに点を設定していく
	VertexData* vertexDataT = nullptr;
	//書き込むためのアドレスを取得
	vertexResourceTri->Map(0, nullptr,
		reinterpret_cast<void**>(&vertexDataT));

	vertexDataT[0].position={-0.5f,-0.5f,0.0f,1.0f};
	vertexDataT[1].position = { 0.0f,0.5f,0.0f,1.0f };
	vertexDataT[2].position = { 0.5f,-0.5f,0.0f,1.0f };

	vertexDataT[0].texcoord = { 0.0f,1.0f };
	vertexDataT[1].texcoord = { 0.5f,0.0f };
	vertexDataT[2].texcoord = { 1.0f,1.0f };

	vertexDataT[0].normal = { 0.0f,0.0f,1.0f };
	vertexDataT[1].normal = vertexDataT[2].normal= vertexDataT[3].normal= vertexDataT[4].normal= vertexDataT[5].normal = vertexDataT[0].normal;

	vertexDataT[3].position = { -0.5f,-0.5f,0.5f,1.0f };
	vertexDataT[4].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexDataT[5].position = { 0.5f,-0.5f,-0.5f,1.0f };

	vertexDataT[3].texcoord = { 0.0f,1.0f };
	vertexDataT[4].texcoord = { 0.5f,0.0f };
	vertexDataT[5].texcoord = { 1.0f,1.0f };
#pragma endregion
#pragma region wvp
	//WVP用のリソースを作る。Matrix４ｘ４1つ分のサイズを用意する
	ID3D12Resource* wvpResourceTri = CreateBufferResource(DXF->GetDevice(), sizeof(WorldTransformation));
	//データを書き込む
	WorldTransformation* wvpDataTri = nullptr;
	//書き込むためのアドレスを取得
	wvpResourceTri->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataTri));
	//単位行列を書き込んでおくtextureResource
	wvpDataTri->WVP = MakeIdentity4x4();
	wvpDataTri->World = MakeIdentity4x4();
#pragma endregion
#pragma region マテリアル
	ID3D12Resource* materialTriangle = CreateBufferResource(DXF->GetDevice(), sizeof(Material));
	Material* materialT = nullptr;
	materialTriangle->Map(0, nullptr, reinterpret_cast<void**>(&materialT));
	materialT->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);;
	materialT->enableHalfLambert = false;
	materialT->enableLighting = false;
	materialT->enableTexture = true;
	materialT->uvTransform = MakeIdentity4x4();
#pragma endregion

#pragma endregion
#pragma region TransformationMatrix用のResourceを作る
	//WVP用のリソースを作る。Matrix４ｘ４1つ分のサイズを用意する
	ID3D12Resource* wvpResource = CreateBufferResource(DXF->GetDevice(), sizeof(WorldTransformation));
	//データを書き込む
	WorldTransformation* wvpData = nullptr;
	//書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおくtextureResource
	wvpData->WVP = MakeIdentity4x4();
	wvpData->World = MakeIdentity4x4();
#pragma endregion
*/

#pragma region Material用のResourceを作る
/*
	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	ID3D12Resource* materialResource = CreateBufferResource(DXF->GetDevice(), sizeof(Material));
	//マテリアルにデータを書き込む
	Material* materialData = nullptr;
	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = true;
	materialData->uvTransform = MakeIdentity4x4();
	materialData->enableTexture = false;
	materialData->enableHalfLambert = true;
	

	//ディレクションライトのマテリアルリソース
	ID3D12Resource* directionalLightResource = CreateBufferResource(DXF->GetDevice(), sizeof(DirectionalLight));
	DirectionalLight* directionalLightData = nullptr;
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	directionalLightData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	directionalLightData->direction = Vector3(0.0f,-1.0f,0.0f);
	directionalLightData->intensity = 1.0f;

	*/
#pragma endregion
int texture = TextureManager::LoadTex("resources/uvChecker.png");

int monstarBall = TextureManager::LoadTex("resources/monsterBall.png");

#pragma region textureの読み込み
	//DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
	/*
	
	
	DirectX::ScratchImage mipImages = LoadTexture(modeldata.material.textureFilePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ID3D12Resource* textureResource = CreateTextureResource(DXF->GetDevice(), metadata);
	ID3D12Resource* intermediateResource = UploadTextureData(textureResource, mipImages, DXF->GetDevice(), DXF->GetCMDList());
	
		//metadataにSRVの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dtexture
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

		//SRVを作成するDescriptorHeapの場所を決める
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = DirectXFunc::GetCPUDescriptorHandle(DXF->GetSRV(), DXF->GetSRVsize(), 1);
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(DXF->GetSRV(), DXF->GetSRVsize(), 1);
		//srvの生成
		DXF->GetDevice()->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);
	*/
	
	/*
	DirectX::ScratchImage mipImages2 = LoadTexture("resources/monsterBall.png");
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	ID3D12Resource* textureResource2 = CreateTextureResource(DXF->GetDevice(), metadata2);
	ID3D12Resource* intermediateResource2 = UploadTextureData(textureResource2, mipImages2, DXF->GetDevice(), DXF->GetCMDList());
	
		//metadataにSRVの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
		srvDesc2.Format = metadata2.format;
		srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dtexture
		srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

		//SRVを作成するDescriptorHeapの場所を決める
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = DirectXFunc::GetCPUDescriptorHandle(DXF->GetSRV(), DXF->GetSRVsize(), 2);
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2= GetGPUDescriptorHandle(DXF->GetSRV(), DXF->GetSRVsize(), 2);
		//srvの生成
		DXF->GetDevice()->CreateShaderResourceView(textureResource2, &srvDesc2, textureSrvHandleCPU2);
	*/
#pragma endregion


#pragma region ShaderResourceView(SRV)
	
	
	
#pragma endregion



	DXF->KickCommand();

	//GPUに送ったので殺す
	//intermediateResource->Release();
	//intermediateResource2->Release();


	ImGuiManager* imguiManager = nullptr;
	imguiManager = new ImGuiManager;
	imguiManager->Initialize(winApp, DXF);

#pragma region 更新

#pragma region メイン
	//カメラ情報
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };
	//Transform変数を作る
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//Transform変数を作る
	Transform OBJT{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Transform teapot{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };


	//リソースのtransform
	Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Transform UVT{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Transform triangle{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	bool useMonsterBall = true;
	bool useShader3D = true;
	bool useTexture = true;
	bool useHalfLambert = true;
	enum Mode {
		Triangle,
		Sphere,
		Sprite,
		Model,
	};

	Mode mode = Model;

	int num = Triangle;
#pragma endregion

	while (winApp->ProcessMessage())
	{	
		imguiManager->PreUpdate();
		//ゲーム内処理
#pragma region 更新処理
		//開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に書き換える
		ImGui::ShowDemoWindow();
#pragma region 全部の更新処理とCBuffer転送

			ImGui::Begin("Camera");
			ImGui::DragFloat3("Camera translate", &cameraTransform.translate.x, 0.01f);
			ImGui::DragFloat3("Camera rotate", &cameraTransform.rotate.x, 0.01f);
			ImGui::Checkbox("useTexture", &useTexture);
			ImGui::Checkbox("useMonsterBall", &useMonsterBall);
			ImGui::DragInt("scene", &num, 1, 0, 3);

			ImGui::End();
			if (useTexture) {
				//materialData->enableTexture = true;
				//materialT->enableTexture = true;

				sprite->IsEnableTexture(true);
				
			}
			else {
				//materialData->enableTexture = false;
				//materialT->enableTexture = false;
				
				sprite->IsEnableTexture(false);

			}

			//カメラ処理
			Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrix = Inverse(cameraMatrix);
			//透視投影行列
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
			Matrix4x4 VP =Multiply(viewMatrix, projectionMatrix);


#pragma region UV
			ImGui::Begin("UV");
			ImGui::DragFloat2("trans", &UVT.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("scale", &UVT.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("rotate", &UVT.rotate.z);
			ImGui::End();
			Matrix4x4 worldUV = MakeAffineMatrix(UVT.scale, UVT.rotate, UVT.translate);

			//materialData->uvTransform = worldUV;

			sprite->SetUV_T(worldUV);

#pragma endregion
			/*
			switch (num)
			{
			case Triangle:
			
#pragma region Triangle
				triangle.rotate.y += 0.01f;
				ImGui::Begin("Triangle");
				ImGui::DragFloat3("trans", &triangle.translate.x, 0.01f);
				ImGui::DragFloat3("rotate", &triangle.rotate.x, 0.01f);
				ImGui::DragFloat3("scale", &triangle.scale.x, 0.01f);
				ImGui::ColorEdit4("color", &materialT->color.x);
				ImGui::End();
				Matrix4x4 WM = MakeAffineMatrix(triangle.scale, triangle.rotate, triangle.translate);
				Matrix4x4 WVPTri = Multiply(WM, VP);
				wvpDataTri->WVP = WVPTri;
				wvpDataTri->World = WM;
#pragma endregion		
				break;
			
			case Sphere:				
#pragma region 円
				transform.rotate.y += 0.03f;
				ImGui::Begin("Sphere");
				ImGui::DragFloat3("pos", &transform.translate.x, 0.01f);
				ImGui::DragFloat3("rotate", &transform.rotate.x, 0.01f);
				ImGui::DragFloat3("scale", &transform.scale.x, 0.01f);
				ImGui::ColorEdit4("color", &materialData->color.x);
				ImGui::End();
				//ワールド
				Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
				Matrix4x4 WVP = Multiply(worldMatrix, VP);
				wvpDataS->WVP = WVP;
				wvpDataS->World = worldMatrix;
#pragma endregion
#pragma region 影
				ImGui::Begin("directional Light");
				ImGui::DragFloat3("direction", &directionalLightData->direction.x, 0.01f);
				ImGui::DragFloat("power", &directionalLightData->intensity, 0.01f);
				ImGui::ColorEdit4("color", &directionalLightData->color.x);
				ImGui::Checkbox("shader", &useShader3D);
				ImGui::End();
				directionalLightData->direction = Normalize(directionalLightData->direction);
				if (useShader3D) {
					materialData->enableLighting = true;
				}
				else {
					materialData->enableLighting = false;
				}

				if (useHalfLambert) {
					materialData->enableHalfLambert = true;
				}
				else {
					materialData->enableHalfLambert = false;
				}
#pragma endregion				
				break;
			case Sprite:			

#pragma region スプライト

				Vector4 color = sprite->GetMaterialData().color;

				ImGui::Begin("Sprite");
				ImGui::DragFloat3("trans", &transformSprite.translate.x, 1.0f);
				ImGui::DragFloat3("scale", &transformSprite.scale.x, 0.01f);
				ImGui::DragFloat3("rotate", &transformSprite.rotate.x, 0.01f);
				ImGui::ColorEdit4("color", &color.x);
				ImGui::End();
				
				sprite->SetMaterialDataColor(color);

#pragma endregion		
				break;
			case Model:
#pragma region オブジェクト更新
				ImGui::Begin("bunny");
				ImGui::DragFloat3("trans", &OBJT.translate.x, 0.01f);
				ImGui::DragFloat3("rotate", &OBJT.rotate.x, 0.01f);
				ImGui::DragFloat3("scale", &OBJT.scale.x, 0.01f);
				ImGui::ColorEdit4("color", &materialData->color.x);
				ImGui::End();
				Matrix4x4 WOBJ = MakeAffineMatrix(OBJT.scale, OBJT.rotate, OBJT.translate);
				Matrix4x4 WVPOBJ = Multiply(WOBJ, VP);

				//データを転送
				wvpData->WVP = WVPOBJ;
				wvpData->World = WOBJ;

				ImGui::Begin("teapot");
				ImGui::DragFloat3("trans", &teapot.translate.x, 0.01f);
				ImGui::DragFloat3("rotate", &teapot.rotate.x, 0.01f);
				ImGui::DragFloat3("scale", &teapot.scale.x, 0.01f);
				ImGui::ColorEdit4("color", &materialData->color.x);
				ImGui::End();
				Matrix4x4 Wtea = MakeAffineMatrix(teapot.scale, teapot.rotate, teapot.translate);
				Matrix4x4 WVPtea = Multiply(Wtea, VP);
				wvpDataTea->World = Wtea;
				wvpDataTea->WVP = WVPtea;
#pragma endregion
#pragma region 影
				ImGui::Begin("directional Light");
				ImGui::DragFloat3("direction", &directionalLightData->direction.x, 0.01f);
				ImGui::DragFloat("power", &directionalLightData->intensity, 0.01f);
				ImGui::ColorEdit4("color", &directionalLightData->color.x);
				ImGui::Checkbox("shader", &useShader3D);
				ImGui::Checkbox("Half Lambert", &useHalfLambert);
				ImGui::End();
				directionalLightData->direction = Normalize(directionalLightData->direction);
				if (useShader3D) {
					materialData->enableLighting = true;
				}
				else {
					materialData->enableLighting = false;
				}
				if (useHalfLambert) {
					materialData->enableHalfLambert = true;
				}
				else {
					materialData->enableHalfLambert = false;
				}
#pragma endregion				
				break;
			default:
				break;
			}
			*/

#pragma endregion
#pragma endregion

		imguiManager->PostUpdate();
#pragma region 描画
			
#pragma region コマンドを積み込んで確定させる
		DXF->PreDraw();
		imguiManager->PreDraw();
		graphics->PreDraw(DXF->GetCMDList());
		
		
#pragma region 描画コマンド
			switch (num)
			{
			case Triangle:
			
#pragma region 三角
				/*
				DXF->GetCMDList()->IASetVertexBuffers(0, 1, &vertexBufferViewTri);
				//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
				DXF->GetCMDList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				//wvp用のCBufferの場所の設定
				DXF->GetCMDList()->SetGraphicsRootConstantBufferView(1, wvpResourceTri->GetGPUVirtualAddress());
				//マテリアルCBufferの場所を設定
				DXF->GetCMDList()->SetGraphicsRootConstantBufferView(0, materialTriangle->GetGPUVirtualAddress());
				//
				//DXF->GetCMDList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
				//SRVのDescriptorTableの先頭を設定。２はParameter[2]である。
				DXF->GetCMDList()->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
				//描画！		
				DXF->GetCMDList()->DrawInstanced(pointT, 1, 0, 0);
				*/
#pragma endregion

				
				break;

			case Sphere:
				
				//RootSignatureを設定。PSOに設定しているけど別途設定が必要
#pragma region 円
				/*
				DXF->GetCMDList()->IASetVertexBuffers(0, 1, &vertexBufferViewSphere);
				//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
				DXF->GetCMDList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				//wvp用のCBufferの場所の設定
				DXF->GetCMDList()->SetGraphicsRootConstantBufferView(1, wvpResourceS->GetGPUVirtualAddress());
				//マテリアルCBufferの場所を設定
				DXF->GetCMDList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
				//
				DXF->GetCMDList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
				//SRVのDescriptorTableの先頭を設定。２はParameter[2]である。
				DXF->GetCMDList()->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
				//描画！		
				DXF->GetCMDList()->DrawInstanced(point, 1, 0, 0);
				*/
#pragma endregion			
				break;
			case Sprite:
#pragma region 2D描画コマンド		
				//RootSignatureを設定。PSOに設定しているけど別途設定が必要
				

				Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
				//スプライト用データ
				Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
				Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
				Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
				//データ転送
				//->WVP = worldViewProjectionMatrixSprite;
				//transformationMatrixDataSprite->World = worldMatrixSprite;

				

				sprite->Draw(worldViewProjectionMatrixSprite,worldMatrixSprite, monstarBall);
				

#pragma endregion
				break;
			case Model:
				
				//RootSignatureを設定。PSOに設定しているけど別途設定が必要
				/*
				DXF->GetCMDList()->IASetVertexBuffers(0, 1, &vertexBufferView);
				//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
				DXF->GetCMDList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				//wvp用のCBufferの場所の設定
				DXF->GetCMDList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
				//マテリアルCBufferの場所を設定
				DXF->GetCMDList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
				//
				DXF->GetCMDList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
				//SRVのDescriptorTableの先頭を設定。２はParameter[2]である。
				DXF->GetCMDList()->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
				//描画！		
				DXF->GetCMDList()->DrawInstanced(UINT(modeldata.vertices.size()), 1, 0, 0);



				DXF->GetCMDList()->IASetVertexBuffers(0, 1, &vertexBufferViewtea);
				//wvp用のCBufferの場所の設定
				DXF->GetCMDList()->SetGraphicsRootConstantBufferView(1, wvpResourceTea->GetGPUVirtualAddress());

				DXF->GetCMDList()->DrawInstanced(UINT(modeltea.vertices.size()), 1, 0, 0);
				*/
				break;
			default:
				break;
			}

			
			//コマンドを組むおわり
#pragma endregion
			

		imguiManager->PostDraw();
		DXF->PostDraw();
#pragma endregion
#pragma endregion
		
	}
#pragma endregion
#pragma region 開放処理
	
	//vertexResourceSphere->Release();
	//vertexResourceSprite->Release();
	//transformationMatrixResourceSprite->Release();
	//生成と逆順に飲む
	//wvpResource->Release();
	//textureResource->Release();
	//textureResource2->Release();
	//wvpResourceS->Release();
	
#pragma region 02_00
	//materialTriangle->Release();
	//vertexRtea->Release();
	//vertexResourceTri->Release();
	//wvpResourceTri->Release();
	//wvpResourceTea->Release();
	//02_01
	//indexResourceSprite->Release();
	//directionalLightResource->Release();
	//materialSpriteResource->Release();
	//materialResource->Release();
	//vertexResource->Release();
	
	//graphicsPipelineState->Release();
	//signatureBlob->Release();
	//if (errorBlob) {
	//	errorBlob->Release();
	//}
	//rootSignature->Release();
	//pixelShaderBlob->Release();
	//vertexShaderBlob->Release();
#pragma endregion
	//srvDescriptorHeap->Release();
#pragma endregion
#pragma region ReportLiveObjects
	lackCheck.~D3DResourceLeakChecker();
#pragma endregion

	imguiManager->Finalize();	
	DXF->Finalize();	
	winApp->Finalize();


	delete imguiManager;
	

	CoUninitialize();

	return 0;
}