#include "Model.h"

#include"DirectXFunc.h"
#include"function.h"
#include"TextureManager.h"
#include"Log.h"

#include"ImGuiManager.h"

#define _USE_MATH_DEFINES
#include<math.h>
#include<sstream>
#include<cassert>
#include<fstream>



struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

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
	
	std::ifstream file(directoryPath + "/" + filename+".obj");
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
			materialFilename = filename + ".mtl";
			//基本的にobjファイルと同一改装にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modeldata.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}


	}
#pragma endregion
#pragma region 4.ModelDataを返す
	return modeldata;
#pragma endregion
}

void Model::Initialize(
	std::string name_,
	int point, 
	ID3D12Resource* vertexRtea,
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView, 
	ID3D12Resource* wvpResource,
	WorldTransformation* wvpData, 
	ID3D12Resource* materialResourceS, 
	Material* materialData,
	ID3D12Resource* directionalLightResource)
{

	DXF_ = DirectXFunc::GetInstance();

	grarphics_ = new GraphicsSystem();
	grarphics_->Initialize(DXF_->GetDevice());

	name = name_;
	point_ = point;
	vertexRtea_ = vertexRtea;
	vertexBufferView_ = vertexBufferView;
	wvpData_ = wvpData;
	wvpResource_ = wvpResource;
	materialResource_ = materialResourceS;
	materialData_ = materialData;
	directionalLightResource_ = directionalLightResource;


	Log("Model is Created!\n");
}

Model::~Model() {

	delete grarphics_;

	vertexRtea_->Release();
	wvpResource_->Release();
	materialResource_->Release();
	directionalLightResource_->Release();
}

Model* Model::CreateSphere(float kSubdivision,bool enableLighting)
{
	DirectXFunc*DXF= DirectXFunc::GetInstance();

#pragma region 円
#pragma region VertexBufferViewを作成
	int point = (int)kSubdivision * (int)kSubdivision * 6;

	
	ID3D12Resource* vertexResourceSphere = CreateBufferResource(DXF->GetDevice(), sizeof(VertexData) * point);

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
			vertexDataS[start].texcoord = { (lonIndex) / (float)kSubdivision,1.0f - (latIndex) / (float)kSubdivision };
			vertexDataS[start].normal = { vertexDataS[start].position.x,vertexDataS[start].position.y,vertexDataS[start].position.z };

			//2(b)
			vertexDataS[start + 1].position = { cos(lat + kLatEvery) * cos(lon),sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon),1.0f };
			vertexDataS[start + 1].texcoord = { (lonIndex) / (float)kSubdivision,1.0f - (latIndex + 1) / (float)kSubdivision };
			vertexDataS[start + 1].normal = { vertexDataS[start + 1].position.x,vertexDataS[start + 1].position.y,vertexDataS[start + 1].position.z };

			//3(c)
			vertexDataS[start + 2].position = { cos(lat) * cos(lon + kLonEvery),sin(lat),  cos(lat) * sin(lon + kLonEvery),1.0f };
			vertexDataS[start + 2].texcoord = { (lonIndex + 1) / (float)kSubdivision,1.0f - (latIndex) / (float)kSubdivision };
			vertexDataS[start + 2].normal = { vertexDataS[start + 2].position.x,vertexDataS[start + 2].position.y,vertexDataS[start + 2].position.z };

			//二枚目
			//
			vertexDataS[start + 3] = vertexDataS[start + 1];
			//
			vertexDataS[start + 4].position = { cos(lat + kLatEvery) * cos(lon + kLonEvery),sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon + kLonEvery),1.0f };
			vertexDataS[start + 4].texcoord = { (lonIndex + 1) / (float)kSubdivision,1.0f - (latIndex + 1) / (float)kSubdivision };
			vertexDataS[start + 4].normal = { vertexDataS[start + 4].position.x,vertexDataS[start + 4].position.y,vertexDataS[start + 4].position.z };
			//
			vertexDataS[start + 5] = vertexDataS[start + 2];
		}
	}
#pragma endregion
#pragma region wvp設定

	ID3D12Resource* wvpResourceS;

	//WVP用のリソースを作る。Matrix４ｘ４1つ分のサイズを用意する
	wvpResourceS = CreateBufferResource(DXF->GetDevice(), sizeof(WorldTransformation));
	//データを書き込む
	WorldTransformation* wvpDataS = nullptr;

	//書き込むためのアドレスを取得
	wvpResourceS->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataS));
	//単位行列を書き込んでおくtextureResource
	wvpDataS->WVP = MakeIdentity4x4();
	wvpDataS->World = MakeIdentity4x4();
#pragma endregion	
#pragma endregion	
#pragma region マテリアル
	ID3D12Resource* materialResource;

	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	 materialResource = CreateBufferResource(DXF->GetDevice(), sizeof(Material));
	//マテリアルにデータを書き込む
	 Material* materialData = nullptr;
	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = enableLighting;
	materialData->uvTransform = MakeIdentity4x4();
	materialData->enableTexture = true;
	materialData->enableHalfLambert = true;
#pragma endregion
#pragma region ライト
	//ディレクションライトのマテリアルリソース
	ID3D12Resource* directionalLightResource = CreateBufferResource(DXF->GetDevice(), sizeof(DirectionalLight));
	DirectionalLight* directionalLightData = nullptr;
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	directionalLightData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	directionalLightData->direction = Vector3(0.0f, -1.0f, 0.0f);
	directionalLightData->intensity = 1.0f;
#pragma endregion



	Model* model = new Model();
	model->Initialize("Sphere", point,vertexResourceSphere, vertexBufferViewSphere, wvpResourceS, wvpDataS, materialResource, materialData, directionalLightResource);
	
	
	
	return model;

}

Model* Model::CreateFromOBJ(const std::string& filePath)
{
	DirectXFunc* DXF = DirectXFunc::GetInstance();

#pragma region モデル
	ModelData modeltea = LoadObjFile("resources", filePath);

	
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
#pragma region マテリアル
	ID3D12Resource* materialResource;

	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource = CreateBufferResource(DXF->GetDevice(), sizeof(Material));
	//マテリアルにデータを書き込む
	Material* materialData = nullptr;
	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = true;
	materialData->uvTransform = MakeIdentity4x4();
	materialData->enableTexture = true;
	materialData->enableHalfLambert = true;
#pragma endregion
#pragma region ライト
	//ディレクションライトのマテリアルリソース
	ID3D12Resource* directionalLightResource = CreateBufferResource(DXF->GetDevice(), sizeof(DirectionalLight));
	DirectionalLight* directionalLightData = nullptr;
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	directionalLightData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	directionalLightData->direction = Vector3(0.0f, -1.0f, 0.0f);
	directionalLightData->intensity = 1.0f;
#pragma endregion

	Model* model =new Model();
	model->Initialize(filePath,UINT(modeltea.vertices.size()),vertexRtea, vertexBufferViewtea, wvpResourceTea, wvpDataTea, materialResource,materialData,directionalLightResource);


	
	
	return model;
}



void Model::Draw(const Matrix4x4& worldMatrix,const Matrix4x4& viewProjection,int texture)
{
	grarphics_->PreDraw(DXF_->GetCMDList());

	Matrix4x4 WVP = worldMatrix* viewProjection;

	wvpData_->WVP = WVP;
	wvpData_->World = worldMatrix;

	DXF_->GetCMDList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
	DXF_->GetCMDList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//wvp用のCBufferの場所の設定
	DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//マテリアルCBufferの場所を設定
	DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//
	DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定。２はParameter[2]である。
	DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureDescriptorHandle(texture));
	//描画！		
	DXF_->GetCMDList()->DrawInstanced(point_, 1, 0, 0);
}

void Model::DebugParameter()
{
	bool useTexture = materialData_->enableTexture;
	bool useShader = materialData_->enableLighting;
	Vector4 color = materialData_->color;

	ImGui::Begin("status");
	ImGui::Checkbox("Texture", &useTexture);
	ImGui::Checkbox("Shader", &useShader);
	ImGui::End();
}



