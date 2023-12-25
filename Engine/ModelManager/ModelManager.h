#pragma once
#include"struct.h"
#include<vector>
#include<string>
#include<iostream>
#include<unordered_map>

#include"WorldTransform/WorldTransform.h"
#include"Graphics/Graphics.h"
#include"DirectXFunc/DirectXFunc.h"
#include"struct.h"


class ModelManager {

public://シングルトンパターン
	static ModelManager* GetInstance();

private://シングルトンパターン

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager& o) = delete;
	const ModelManager& operator=(const ModelManager& o) = delete;


public:	


	void LoadAllModels();

	

	/// <summary>
	/// モデルデータの取得
	/// </summary>
	/// <param name="filename">modelの名前またはresources以降のパス</param>
	/// <returns>モデルデータ</returns>
	ModelData GetModelData( const std::string& filename);
private:
	//モデルデータのパスが記入されたファイルへのパス
	const std::string& modelPathFile = "resources/modelPathFile.json";
	
	//モデルデータのパス群
	const std::string& groupName = "modelPathFile";

	//データ保存の構造体
	struct NameAndPath {
		std::string name;
		std::string path;
	};

	//保存データ構造
	std::vector<std::pair<NameAndPath, ModelData>> modelDatas;

	//最大量設定
	const size_t maxModelData = 256;

	//GetModelDataでモデルが見つからない場合エラーをはくか否か
	bool isError = true;
};





//インスタンシングでのモデルマネージャ
class InstancingModelManager {

public://シングルトンパターン
	static InstancingModelManager* GetInstance();

private://シングルトンパターン

	InstancingModelManager() = default;
	~InstancingModelManager() = default;
	InstancingModelManager(const InstancingModelManager& o) = delete;
	const InstancingModelManager& operator=(const InstancingModelManager& o) = delete;

public:

	/// <summary>
	/// タグが存在するか検索
	/// </summary>
	/// <param name="tag"></param>
	/// <returns></returns>
	bool SerchTag(const std::string& tag);

	/// <summary>
	/// ワールドデータを受け取る
	/// </summary>
	/// <param name="tag"></param>
	/// <param name="matworld"></param>
	void GivenWorldData(const std::string& tag, const Matrix4x4& matworld);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	void PreUpdate();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// パスのモデルの読み込み
	/// </summary>
	void LoadAllModels();

	//すべてのモデルの描画
	void DrawAllModels(const Matrix4x4& viewProjection);

	

private:

	struct DataGroup {
		std::string path;
		std::string name;
	};

	//ライトのデータ構造体
	struct LightVariables {
		ID3D12Resource* directionalLightResource = nullptr;//ライト
		DirectionalLight* dLData;				//ライトのデータ
	};

	//モデル描画に必要なデータ群
	struct ModelVariable {
		D3D12_GPU_DESCRIPTOR_HANDLE texture;	//画像データ
		WorldTransform* uvWorld;					//uvの行列
		int32_t point;							//頂点
		ID3D12Resource* vertexData;				//頂点データ
		D3D12_VERTEX_BUFFER_VIEW vBv{};			//vertexBufferView
		int32_t instancingNum;					//インスタンシング描画する最大数
		ID3D12Resource* wvpResource;			//wvpResource
		WorldTransformation *wvpData;			//wvpData
		ID3D12Resource* materialResource;		//マテリアル
		Material* materialData;					//マテリアルのデータ
		LightVariables lightData;				//ライトのデータ
		D3D12_GPU_DESCRIPTOR_HANDLE instancingHandle;//インスタンシングのハンドル
		std::vector<Matrix4x4>worlds;//ワールドデータ保存位置
	};

	void CreateModelData(const std::string&tag,const DataGroup&filepass,int instancingNum);

private:

	//DXF
	DirectXFunc* DXF_=nullptr;

	//3DInstancingのGraphicsを作成
	std::unique_ptr<InstancingGraphicsSystem> instancingPSO_;


	//モデルデータのパスが記入されたファイルへのパス
	const std::string& modelPathFile = "resources/modelPathFile.json";

	//モデルデータのパス群
	const std::string& groupName = "InstancingmodelPathFile";

	//ライトのデータはほぼ一緒なのでコピーして渡す
	std::shared_ptr<LightVariables> lightData_;

	//データタグとそのモデルデータ

	std::unordered_map<std::string, ModelVariable*>modelDatas_;

	//最大量設定
	const size_t maxModelData = 256;

	//GetModelDataでモデルが見つからない場合エラーをはくか否か
	bool isError = true;
};