#pragma once

#include<string>
#include<vector>
#include<iostream>
#include<InstancingModel/InstancingModel.h>

class InstancingModelManager {
public:
	static InstancingModelManager* GetInstance();
private:
	InstancingModelManager() = default;
	~InstancingModelManager() = default;
	InstancingModelManager(const InstancingModelManager& o) = delete;
	const InstancingModelManager& operator=(const InstancingModelManager& o) = delete;

public:

	/// <summary>
	/// すべてのモデルのロード
	/// </summary>
	void LoadAllModel();

	/// <summary>
	/// 更新前処理
	/// </summary>
	void PreUpdate();

	/// <summary>
	/// すべてのモデルの描画
	/// </summary>
	void DrawAllModel(const Matrix4x4& viewProjection);

	
	/// <summary>
	/// タグの検索
	/// </summary>
	/// <param name="tag"></param>
	bool SerchTag(const std::string& tag);

	void SetWorld(const std::string& tag, const WorldTransform& world);
private:

	//jsonのパス
	const std::string modelPathFile_ = "resources/modelPathFile.json";

	//グループ名
	const std::string groupName_ = "InstancingmodelPathFile";

	//データ
	std::vector<std::unique_ptr<InstancingModel>>modeldatas_;

	//データの最大読み込み量
	const int maxModelNum = 256;
	//モデルの読み込んだ数
	int modelNum = 0;

};