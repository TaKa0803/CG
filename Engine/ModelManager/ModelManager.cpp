#include "ModelManager.h"

#include<cassert>
#include<fstream>
#include<json.hpp>

#include<Windows.h>
#include<winuser.h>

#include"functions/function.h"
#include"TextureManager/TextureManager.h"
#include"SRVManager/SRVManager.h"

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	//1中で必要になる変数の宣言
	MaterialData materialdata;

	std::string line;
	//２ファイルを開く
	std::ifstream file(directoryPath + "/" + filename + "/" + filename + ".mtl");
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
			materialdata.textureFilePath = directoryPath + "/" + filename + "/" + textureFilename;
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

	std::ifstream file(directoryPath + "/" + filename+"/"+filename + ".obj");
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
			materialFilename = filename;
			//基本的にobjファイルと同一改装にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modeldata.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}


	}
#pragma endregion
#pragma region 4.ModelDataを返す
	return modeldata;
#pragma endregion
}

ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;
	return &instance;
}

void ModelManager::LoadAllModels() {
	//サイズを設定
	modelDatas.resize(maxModelData);

	//読み込むjsonファイル
	std::string filepath = modelPathFile;
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filepath);

	//ファイルオープン失敗処理
	if (ifs.fail()) {
		std::string message = "モデルパス記載のjsonファイルが見つかりません！";
		MessageBoxA(nullptr, message.c_str(), "ModelManager", 0);
		assert(message.c_str());
		return;
	}

	nlohmann::json root;
	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//移したのでファイルを閉じる
	ifs.close();

	//グループ検索
	nlohmann::json::iterator itGroup = root.find(groupName);

	//未登録チェック
	assert(itGroup != root.end());

	//各アイテムについて
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテムの名前を取得
		const std::string& itemName = itItem.key();

		if (itItem->is_array()&&itItem->size()==2) {
			//モデル群の八田フォルダまでのパス
			std::string foldaPath = itItem->at(0);

			//パスを取得
			std::string modelPath = itItem->at(1);


			//名前とパスを合わせた構造体
			NameAndPath nameAndPath = { itemName,modelPath };

			//モデルデータを作成して設定
			ModelData newmodelData = LoadObjFile(foldaPath, modelPath);
			std::pair<NameAndPath, ModelData>newData(nameAndPath, newmodelData);
			modelDatas.emplace_back(newData);

		}
	}



}

ModelData ModelManager::GetModelData(const std::string& filename) {
	//フルパスを作成
	std::string name = filename;

	//データ型に該当するものを追加
	for (auto& modeldata : modelDatas) {
		//名前が同じorパスが同じ場合
		if (modeldata.first.name == name || modeldata.first.path == name) {
			return modeldata.second;
		}
	}
	//存在しない場合の処理
	if (isError) {
		//存在していないのでエラー
		assert(false);
	}
	else {
		//モデルデータを作成して設定
		ModelData newmodelData = LoadObjFile("resources", filename);
		std::pair<std::string, ModelData>newData(name, newmodelData);
		modelDatas.emplace_back(newData);
		return modelDatas.back().second;
	}


	return ModelData();
}


InstancingModelManager* InstancingModelManager::GetInstance() {
	
	static InstancingModelManager instance;
	return &instance;

}

void InstancingModelManager::Initialize() {
	//DirectXFuncのインスタンス取得
	DXF_ = DirectXFunc::GetInstance();

	//Grapgics設定
	instancingPSO_->Initialize(DXF_->GetDevice());


	//ライト情報の初期化
#pragma region ライト
	lightData_ = std::make_shared<LightVariables>();
	//ディレクションライトのマテリアルリソース
	lightData_->directionalLightResource = CreateBufferResource(DXF_->GetDevice(), sizeof(DirectionalLight));
	lightData_->directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&lightData_->dLData));
	//各データ初期設定
	lightData_->dLData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	lightData_->dLData->direction = Vector3(0.0f, -1.0f, 0.0f);
	lightData_->dLData->intensity = 1.0f;
#pragma endregion


}


void InstancingModelManager::PreUpdate() {
	//すべてのワールドデータを初期化
	for (auto& modeldata : modelDatas_) {
		modeldata.second->worlds.clear();
	}
}

void InstancingModelManager::Finalize() {

}

void InstancingModelManager::LoadAllModels() {
	

	//読み込むjsonファイル
	std::string filepath = modelPathFile;
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filepath);

	//ファイルオープン失敗処理
	if (ifs.fail()) {
		std::string message = "モデルパス記載のjsonファイルが見つかりません！";
		MessageBoxA(nullptr, message.c_str(), "ModelManager", 0);
		assert(message.c_str());
		return;
	}

	nlohmann::json root;
	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//移したのでファイルを閉じる
	ifs.close();

	//グループ検索
	nlohmann::json::iterator itGroup = root.find(groupName);

	//未登録チェック
	assert(itGroup != root.end());

	//各アイテムについて
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテムの名前を取得
		const std::string& itemName = itItem.key();

		if (itItem->is_array() && itItem->size() == 3) {
			//モデルファイルをまとめたフォルダまでのパス
			std::string foldaPath = itItem->at(0);

			//フォルダ名を取得
			std::string modelPath = itItem->at(1);

			//数を取得
			int modelNum = itItem->at(0);

			//名前とパスを合わせた構造体
			DataGroup nameAndPath = { itemName,modelPath };

			//モデルデータを作成して設定
			CreateModelData(nameAndPath, modelNum);

			

		}
	}

}

void InstancingModelManager::CreateModelData(const DataGroup& filepass, int instancingNum) {

	//モデルデータ読み込み
	ModelData modeldata = LoadObjFile(filepass.path, filepass.name);

	//モデルの処理に必要なデータを用意
	std::unique_ptr<ModelVariable> modelVariable = std::make_unique<ModelVariable>();

	//読み込んだデータから各種作成

#pragma region 画像データ
	SRVManager* SRVM = SRVManager::GetInstance();
	int tex;
	//画像指定がない場合
	if (modeldata.material.textureFilePath == "") {
		//仮画像を渡す
		tex = TextureManager::uvChecker_;
	}
	else {
		//ある場合
		//画像読み込み
		tex = TextureManager::LoadTex(modeldata.material.textureFilePath);
	}
	//配列位置から画像を取得
	modelVariable->texture = SRVM->GetTextureDescriptorHandle(tex);

#pragma endregion
#pragma region vertex関係
	//頂点数設定
	modelVariable->point = (int32_t)modeldata.vertices.size();
	//vertex関係
	modelVariable->vertexData = CreateBufferResource(DXF_->GetDevice(), sizeof(VertexData) * modeldata.vertices.size());
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewtea{};
	vertexBufferViewtea.BufferLocation = modelVariable->vertexData->GetGPUVirtualAddress();
	vertexBufferViewtea.SizeInBytes = UINT(sizeof(VertexData) * modeldata.vertices.size());
	vertexBufferViewtea.StrideInBytes = sizeof(VertexData);

	VertexData* vertexDatatea = nullptr;
	modelVariable->vertexData->Map(0, nullptr, reinterpret_cast<void**>(&vertexDatatea));
	std::memcpy(vertexDatatea, modeldata.vertices.data(), sizeof(VertexData) * modeldata.vertices.size());
#pragma endregion
#pragma region WVP関連
	//WVP用のリソースを作る。Matrix４ｘ４1つ分のサイズを用意する
	modelVariable->wvpResource = CreateBufferResource(DXF_->GetDevice(), sizeof(WorldTransformation) * instancingNum);
	//データを書き込む
	//書き込むためのアドレスを取得
	modelVariable->wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&modelVariable->wvpData));
	//単位行列を書き込んでおくtextureResource
	for (uint32_t index = 0; index < (uint32_t)instancingNum; ++index) {
		modelVariable->wvpData[index].WVP = MakeIdentity4x4();
		modelVariable->wvpData[index].World = MakeIdentity4x4();
	}
#pragma endregion
#pragma region マテリアル
	//マテリアル用のリソース
	modelVariable->materialResource = CreateBufferResource(DXF_->GetDevice(), sizeof(Material));
	modelVariable->materialResource->Map(0, nullptr, reinterpret_cast<void**>(&modelVariable->materialData));
	//各データ初期設定
	modelVariable->materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	modelVariable->materialData->enableLighting = true;
	modelVariable->materialData->uvTransform = MakeIdentity4x4();
	modelVariable->materialData->enableTexture = true;
	modelVariable->materialData->enableHalfLambert = true;
	modelVariable->materialData->discardNum = 0.0f;
#pragma endregion

	//ライトデータをコピー
	modelVariable->lightData = *lightData_.get();

#pragma region instancing関係のデータ
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingDesc{};
	instancingDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingDesc.Buffer.FirstElement = 0;
	instancingDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingDesc.Buffer.NumElements = instancingNum;
	instancingDesc.Buffer.StructureByteStride = sizeof(WorldTransformation);

	
	int instancingHandleNum = SRVM->CreateSRV(modelVariable->wvpResource, nullptr, instancingDesc);

	//配列位置から画像を取得
	modelVariable->instancingHandle = SRVM->GetTextureDescriptorHandle(instancingHandleNum);

#pragma endregion

	//タグ名保存
	std::string tag = filepass.name;

	std::vector<std::unique_ptr<Matrix4x4>>worlds;

	std::unique_ptr<WorldData>worldata;
	worldata->modelData = std::move(modelVariable);

	//データをまとめて保存
	std::pair<std::string, std::unique_ptr<WorldData>>newData(tag, std::move(worldata));

	modelDatas_.insert(std::move(newData));
}

void InstancingModelManager::DrawAllModels(const Matrix4x4& viewProjection) {
	//全モデルの描画
	for (auto& model : modelDatas_) {
		
		//world群を送信
		int index = 0;
		for (auto& world : model.second->worlds) {
			//WVP作成
			Matrix4x4 WVP = *world.get() * viewProjection;

			//world,WVPを代入
			model.second->modelData->wvpData[index].World = *world.get();
			model.second->modelData->wvpData[index].WVP = WVP;	
			index++;
		}

		//ワールド数が1つでもあるとき
		if (index != 0) {

			//UVWorldの更新と行列の代入
			model.second->modelData->materialData->uvTransform = model.second->modelData->uvWorld.UpdateMatrix();

			DXF_->GetCMDList()->IASetVertexBuffers(0, 1, &model.second->modelData->vBv);
			//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
			DXF_->GetCMDList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


			//マテリアルCBufferの場所を設定
			DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(0, model.second->modelData->materialResource->GetGPUVirtualAddress());
			//ライト用の
			DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(1, model.second->modelData->lightData.directionalLightResource->GetGPUVirtualAddress());
			//インスタンシングの座標設定
			DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(2, model.second->modelData->instancingHandle);
			//画像設定
			DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(2, model.second->modelData->texture);
			//描画！		
			DXF_->GetCMDList()->DrawInstanced(model.second->modelData->point, index, 0, 0);

		}
	}
}

