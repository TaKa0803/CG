#include "EnemyPopManager.h"

#include<fstream>
#include<cassert>
#include<Windows.h>
#include<winuser.h>
#include<json.hpp>

#include"RandomNum/RandomNum.h"

void EnemyPopManager::LoadPopdata() {

	std::ifstream ifs;

	ifs.open(filePath);

	//ファイルオープン失敗処理
	if (ifs.fail()) {
		std::string message = "敵出現データ記載のjsonファイルが見つかりません！";
		MessageBoxA(nullptr, message.c_str(), "ModelManager", 0);
		assert(message.c_str());
		return;
	}

	nlohmann::json root;
	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//移したのでファイルを閉じる
	ifs.close();

	//サイズを変更
	popDatas_.resize(areaNum);

	//データ探索と作成
	int iterator = 0;
	for (auto& areaName : popAreaName) {
		//グループ検索
		nlohmann::json::iterator itGroup = root.find(areaName);

		//未登録チェック
		assert(itGroup != root.end());

		//構造体作成
		PopData popData{};

		popData.areaName = areaName;
		//各アイテムについて
		for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
			//アイテムの名前を取得
			const std::string& itemName = itItem.key();

			//各種データ取得
			if (itemNames[(int)INE::POSITION] == itemName) {
				popData.areaPosition = Vector3(itItem->at(0), itItem->at(1), itItem->at(2));
			}
			else if (itemNames[(int)INE::POPAREASIZE] == itemName) {
				popData.popAreaSize = itItem->get<float>();
			}
			else if (itemNames[(int)INE::RANGEPOPSTART] == itemName) {
				popData.PopStartingArea = itItem->get<float>();
			}
			else if (itemNames[(int)INE::POPINTERVAL] == itemName) {
				popData.PopInterval = itItem->get<float>();
			}
			else if (itemNames[(int)INE::MAXAREAPOP] == itemName) {
				popData.maxAreaPop = itItem->get<float>();
			}
			
		}

		popData.PopIntervalCount = 0;
		popData.maxAreaPopCount = 0;

		//データ挿入
		popDatas_[iterator]=popData;

		iterator++;
	}
}

void EnemyPopManager::Update() {

}

std::unique_ptr<Enemy> EnemyPopManager::PopEnemy() {
	
	//プレイヤー登録しているかチェック
	 if (playerWorld_) {

		 //出現エリアごとに処理
		 for (auto& popdata : popDatas_) {

			 //インターバルが０以下＆出現量が最大を超えていない
			 if (popdata.PopIntervalCount-- <= 0&&popdata.maxAreaPopCount<popdata.maxAreaPop) {

				 //出現エリアとの距離測定
				 float length = Length(playerWorld_->translate_ - popdata.areaPosition);

				 //エリア内で出現
				 if (length <= popdata.PopStartingArea) {

					 float area = popdata.popAreaSize;

					 //乱数取得クラス呼び出し
					 RandomNumber* rNum = RandomNumber::GetInstance();

					 //出現座標作成
					 Vector3 newPos{
						 rNum->Get(-area,area),
						 0,
						 rNum->Get(-area,area),
					 };

					 //生成インターバルを更新
					 popdata.PopIntervalCount = popdata.PopInterval;

					 //エリア出現最大数を加算
					 popdata.maxAreaPopCount++;

					 //新しく作成
					 std::unique_ptr<Enemy>newEnemy = std::make_unique<Enemy>();
					 newEnemy->Initialize(newPos);
					 
					 //返却
					 return std::move(newEnemy);
				 }

			 }
		 }

	 }

	 //生成出来ず
	 return nullptr;
}
