#include "InstancingModel.h"

#include"Log/Log.h"
#include"functions/function.h"
#include"SRVManager/SRVManager.h"
#include"TextureManager/TextureManager.h"
#include"ImGuiManager/ImGuiManager.h"
#include"functions/function.h"
#include"Quaternion/Quaternion.h"




InstancingModel::~InstancingModel() {
	delete pso_;

	vertexData_->Release();
	wvpResource_->Release();
	materialResource_->Release();
	directionalLightResource_->Release();
}

//ノードを自作構造体に変換
Node ReadNode(aiNode* node) {

	Node result;
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;	//nodeのlocalMatrixを取得
	aiLocalMatrix.Transpose();							//列ベクトルを行ベクトルに転置
	result.localMatrix.m[0][0]=aiLocalMatrix[0][0];		//ほかの要素も同様に

	result.name = node->mName.C_Str();					//Node名を格納
	result.children.resize(node->mNumChildren);			//子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		//再起的に読んで確保する
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}

InstancingModel* InstancingModel::CreateFromOBJ(const std::string& directory,const std::string& filePath, int instancingNum) {
	DirectXFunc* DXF = DirectXFunc::GetInstance();

#pragma region モデル
	ModelManager* mManager = ModelManager::GetInstance();

	//もでるよみこみ
	Assimp::Importer importer;
	std::string filepath = directory + "/" + filePath;
	const aiScene* scene = importer.ReadFile(filepath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	//メッシュがないのは対応しない
	assert(scene->HasMeshes());

	ModelData modelData;

	//各メッシュ解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());			//法線のないメッシュ比対応
		assert(mesh->HasTextureCoords(0));	//TexCoordがないMeshは比対応

		//メッシュの中井の解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);//三角形のみサポート

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

				

				VertexData vertex;
				vertex.indexID = vertexIndex;
				vertex.position = { position.x,position.y,position.z,1.0f };
				vertex.normal = { normal.x,normal.y,normal.z };
				vertex.texcoord = { texcoord.x,texcoord.y };
				
				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;
				
				//頂点データ送信
				modelData.vertices.push_back(vertex);
			}
		}


		//メッシュのボーン解析
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			//各ボーンに対するウェイト取得
			auto& bone = mesh->mBones[boneIndex];

			//データ作成
			BoneData boneda;
			
			//データ量とサイズ設定
			std::vector<BoneVertexData>boneVertexData;
			boneVertexData.resize(bone->mNumWeights);

			//ボーンの影響の与える頂点
			for (uint32_t boneVertexIndex = 0; boneVertexIndex < bone->mNumWeights; ++boneVertexIndex) {
				
				//データ読み込み
				auto&boneData =	bone->mWeights[boneVertexIndex];

				//データ格納
				 BoneVertexData bvd= {
					.IndexID{boneData.mVertexId},
					.Weight{boneData.mWeight}
				};

				 //データ設定
				 boneVertexData[boneVertexIndex] = bvd;
			}
			
		}
	}

	//マテリアル解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilepath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilepath);
			modelData.material.textureFilePath = directory + "/" + textureFilepath.C_Str();
		}
	}

	//ノード
	modelData.rootNode = ReadNode(scene->mRootNode);

	std::vector<VertexData>animation;

	animation.resize(scene->mNumAnimations);

	
	//各アニメーション解析
	for (uint32_t animationIndex = 0; animationIndex < scene->mNumAnimations; ++animationIndex) {
		//あるアニメーションについて
		
		//アクセス
		aiAnimation* animation = scene->mAnimations[animationIndex];

		//名前保存
		animeData.name = animation->mName.C_Str();
		//継続時間
		animeData.duration = (float)animation->mDuration;
		//超点数
		animeData.boneNum = animation->mNumChannels;
		
		//数に合わせてサイズ取得
		animeData.bones_.resize(animation->mNumChannels);

		isOBJ = false;
		//各ボーンのアニメーションごとの情報取得
		for (uint32_t animeChannelIndex = 0; animeChannelIndex < animation->mNumChannels; ++animeChannelIndex) {
			//ノードデータ取得
			aiNodeAnim* aiNode = animation->mChannels[animeChannelIndex];

		
			int ite =aiNode->mNumPositionKeys-1;

			auto& name = aiNode->mNodeName;
			auto pos = aiNode->mPositionKeys[ite].mValue;
			auto rotation = aiNode->mRotationKeys[ite].mValue;
			auto scale = aiNode->mScalingKeys[ite].mValue;

			std::string stName = name.C_Str();
			Vector3 translateV = { pos.x,pos.y,pos.z };
			Quaternion rotateV = { rotation.x,rotation.y,rotation.z,rotation.w };
			Vector3 scaleV = { scale.x,scale.y,scale.z };

			//情報を作る
			NodeTransformation transformation = {
				.name{stName},
				.translate{translateV},
				.rotate{rotateV},
				.scale{scaleV}
			};

			//データ入れる
			animeData.bones_[animeChannelIndex] = transformation;


		}

	}

	//ModelData modeltea = LoadObjFile(directory,filePath);

	//頂点データ
	ID3D12Resource* vertexRtea = CreateBufferResource(DXF->GetDevice(), sizeof(VertexData) * modelData.vertices.size());
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewtea{};
	vertexBufferViewtea.BufferLocation = vertexRtea->GetGPUVirtualAddress();
	vertexBufferViewtea.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferViewtea.StrideInBytes = sizeof(VertexData);

	VertexData* vertexDatatea = nullptr;
	vertexRtea->Map(0, nullptr, reinterpret_cast<void**>(&vertexDatatea));
	std::memcpy(vertexDatatea, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());


	
#pragma endregion



	InstancingModel* model = new InstancingModel();
	model->Initialize(modelData.material.textureFilePath, UINT(modelData.vertices.size()),instancingNum, vertexRtea, vertexBufferViewtea);




	return model;
}



InstancingModel* InstancingModel::CreateFromGLTF(const std::string& directory, const std::string& filePath, int instancingNum) {
	DirectXFunc* DXF = DirectXFunc::GetInstance();

#pragma region モデル
	ModelManager* mManager = ModelManager::GetInstance();

	//もでるよみこみ
	Assimp::Importer importer;
	std::string filepath = directory + "/" + filePath;
	const aiScene* scene = importer.ReadFile(filepath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	//メッシュがないのは対応しない
	assert(scene->HasMeshes());

	ModelData modelData;

	//メッシュ解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());			//法線のないメッシュ比対応
		assert(mesh->HasTextureCoords(0));	//TexCoordがないMeshは比対応

		//メッシュの中井の解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);//三角形のみサポート

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				VertexData vertex;
				vertex.position = { position.x,position.y,position.z,1.0f };
				vertex.normal = { normal.x,normal.y,normal.z };
				vertex.texcoord = { texcoord.x,texcoord.y };

				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;

				//頂点データ送信
				modelData.vertices.push_back(vertex);
			}
		}
	}

	//マテリアル解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilepath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilepath);
			modelData.material.textureFilePath = directory + "/" + textureFilepath.C_Str();
		}
	}

	//ノード
	modelData.rootNode = ReadNode(scene->mRootNode);
	
	//頂点データ
	ID3D12Resource* vertexRtea = CreateBufferResource(DXF->GetDevice(), sizeof(VertexData) * modelData.vertices.size());
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewtea{};
	vertexBufferViewtea.BufferLocation = vertexRtea->GetGPUVirtualAddress();
	vertexBufferViewtea.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferViewtea.StrideInBytes = sizeof(VertexData);

	VertexData* vertexDatatea = nullptr;
	vertexRtea->Map(0, nullptr, reinterpret_cast<void**>(&vertexDatatea));
	std::memcpy(vertexDatatea, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());



#pragma endregion



	InstancingModel* model = new InstancingModel();
	model->Initialize(modelData.material.textureFilePath, UINT(modelData.vertices.size()), instancingNum, vertexRtea, vertexBufferViewtea);




	return model;
}

void InstancingModel::PreUpdate() {
	worlds_.clear();
}

void InstancingModel::AddWorld(const WorldTransform& world) {
	//データをコピー
	WorldTransform worl = world;

	std::unique_ptr<WorldTransform>newWorld = std::make_unique<WorldTransform>(worl);
	//追加
	worlds_.push_back(std::move(newWorld));
	
}

void InstancingModel::Draw(const Matrix4x4& viewProjection, int texture) {

	pso_->PreDraw(DXF_->GetCMDList());

	uvWorld_.UpdateMatrix();
	materialData_->uvTransform = uvWorld_.matWorld_;

	int index = 0;
	for (auto& world : worlds_) {
		Matrix4x4 worldM = world.get()->matWorld_;

		Matrix4x4 WVP = worldM * viewProjection;

		wvpData_[index].WVP = WVP;
		wvpData_[index].World = worldM;

		index++;
	}

	DXF_->GetCMDList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
	DXF_->GetCMDList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	
	//マテリアルCBufferの場所を設定
	DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//
	DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	if (texture == -1) {
		DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(2, texture_);
	}
	else {
		//SRVのDescriptorTableの先頭を設定。２はParameter[2]である。
		DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(2, SRVManager::GetInstance()->GetTextureDescriptorHandle(texture));
	}
	DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(1, instancingHandle_);

	//描画！		
	DXF_->GetCMDList()->DrawInstanced(point_,index , 0, 0);

}

void InstancingModel::Initialize(
	std::string name,
	int point,
	int instancingNum,
	ID3D12Resource* vertexRtea,
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView) {

	DXF_ = DirectXFunc::GetInstance();

	pso_ = new InstancingPSO();
	pso_->Initialize(DXF_->GetDevice());

	//各データ受け渡し
	point_ = point;
	instancingNum_ = instancingNum;
	vertexData_ = vertexRtea;
	vertexBufferView_ = vertexBufferView;
	
	//WVP用のリソースを作る。Matrix４ｘ４1つ分のサイズを用意する
	wvpResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(WorldTransformation) * instancingNum);
	//データを書き込む
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列を書き込んでおくtextureResource
	for (uint32_t index = 0; index < (uint32_t)instancingNum; ++index) {
		wvpData_[index].WVP = MakeIdentity4x4();
		wvpData_[index].World = MakeIdentity4x4();
	}

#pragma region マテリアル
	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(Material));
	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->enableTexture = true;
	materialData_->enableHalfLambert = true;
	materialData_->discardNum = 0.0f;
#pragma endregion

#pragma region ライト
	//ディレクションライトのマテリアルリソース
	directionalLightResource_ = CreateBufferResource(DXF_->GetDevice(), sizeof(DirectionalLight));
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	directionalLightData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	directionalLightData_->direction = Vector3(0.0f, -1.0f, 0.0f);
	directionalLightData_->intensity = 1.0f;
#pragma endregion

	SRVManager* SRVM = SRVManager::GetInstance();

#pragma region テクスチャ関係
	//スプライトの指定がない場合
	if (name == "") {
		int tex = TextureManager::uvChecker_;
		texture_ = SRVM->GetTextureDescriptorHandle(tex);
	}
	else {
		//指定があった場合
		int texture = TextureManager::LoadTex(name);
		texture_ = SRVM->GetTextureDescriptorHandle(texture);
	}
#pragma endregion
#pragma region instancing関係のデータ
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC instancingDesc{};
		instancingDesc.Format = DXGI_FORMAT_UNKNOWN;
		instancingDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		instancingDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		instancingDesc.Buffer.FirstElement = 0;
		instancingDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		instancingDesc.Buffer.NumElements = instancingNum;
		instancingDesc.Buffer.StructureByteStride = sizeof(WorldTransformation);

		SRVManager* SRVM = SRVManager::GetInstance();
		int instancingHandleNum = SRVM->CreateSRV(wvpResource_, nullptr, instancingDesc);
		instancingHandle_ = SRVM->GetTextureDescriptorHandle(instancingHandleNum);
	}
#pragma endregion

	Log("Model is Created!\n");
}


void InstancingModel::Draw(const Matrix4x4& viewProjection, int texture) {

	pso_->PreDraw(DXF_->GetCMDList());

	uvWorld_.UpdateMatrix();
	materialData_->uvTransform = uvWorld_.matWorld_;

	int index = 0;
	for (auto& world : worlds_) {
		Matrix4x4 worldM = world.get()->matWorld_;

		Matrix4x4 WVP = worldM * viewProjection;

		if (isOBJ_) {
			wvpData_[index].WVP = WVP;
			wvpData_[index].World = worldM;
		}
		else {
			wvpData_[index].WVP = modelData_.rootNode.localMatrix * WVP;
			wvpData_[index].World = modelData_.rootNode.localMatrix * worldM;
		}
		index++;
	}

	DXF_->GetCMDList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
	DXF_->GetCMDList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//マテリアルCBufferの場所を設定
	DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//
	DXF_->GetCMDList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	if (texture == -1) {
		DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(2, texture_);
	}
	else {
		//SRVのDescriptorTableの先頭を設定。２はParameter[2]である。
		DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(2, SRVManager::GetInstance()->GetTextureDescriptorHandle(texture));
	}
	DXF_->GetCMDList()->SetGraphicsRootDescriptorTable(1, instancingHandle_);

	//描画！		
	DXF_->GetCMDList()->DrawInstanced(point_, index, 0, 0);

}

void InstancingModel::DebugParameter(const char* name) {

#ifdef _DEBUG

	ImGui::Begin(name);
	ImGui::Text("Bone");
	Matrix4x4Debug(modelData_.boneDatas[0].offset, name);
	Matrix4x4Debug(modelData_.boneDatas[1].offset, name);
	ImGui::End();
#endif // _DEBUG



}
