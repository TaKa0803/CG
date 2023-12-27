#include "InstancingModelManager.h"

#include<cassert>
#include<fstream>
#include<json.hpp>

#include<Windows.h>
#include<winuser.h>


InstancingModelManager* InstancingModelManager::GetInstance() {
	static InstancingModelManager instance;
	return &instance;
}

void InstancingModelManager::LoadAllModel() {


	//読み込むjsonファイル
	std::string filepath = modelPathFile_;
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
	nlohmann::json::iterator itGroup = root.find(groupName_);

	//未登録チェック
	assert(itGroup != root.end());

	//各アイテムについて
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {

		if (modelNum <= maxModelNum) {
			//アイテムの名前を取得
			const std::string& itemName = itItem.key();

			if (itItem->is_array() && itItem->size() == 3) {
				//モデル群の八田フォルダまでのパス
				std::string foldaPath = itItem->at(0);

				//パスを取得
				std::string modelPath = itItem->at(1);

				//インスタンシング数取得
				int instancingNum = (int)itItem->at(2);


				//モデルデータを作成して設定
				std::unique_ptr<InstancingModel>newModel;
				newModel.reset(InstancingModel::CreateFromOBJ(foldaPath, modelPath, instancingNum));
				newModel->SetTag(itemName);
				//送信
				modeldatas_.push_back(std::move(newModel));

				//カウンター増加
				modelNum++;
			}
		}
		else {
			//モデル読み込みしすぎ！
			assert(false);
		}
	}


}

void InstancingModelManager::PreUpdate() {

	//すべてのモデルのワールドデータ初期化
	for (auto& model : modeldatas_) {
		model->PreUpdate();
	}

}

void InstancingModelManager::DrawAllModel(const Matrix4x4& viewProjection) {

	for (auto& model : modeldatas_) {
		model->Draw(viewProjection);
	}

}

bool InstancingModelManager::SerchTag(const std::string& tag) {

	//タグ検索
	for (auto& model : modeldatas_) {
		//タグが一致した場合
		if(model->GetTag() == tag){
			return true;
		}
	}

	//見つからないのでエラー
	assert(false);
	return false;
}

void InstancingModelManager::SetWorld(const std::string& tag, const WorldTransform& world) {

	//タグのモデルデータ探索
	for (auto& model : modeldatas_) {
		//タグを発見
		if (model->GetTag() == tag) {
			//ワールドを追加
			model->AddWorld(world);
		}
	}

}
