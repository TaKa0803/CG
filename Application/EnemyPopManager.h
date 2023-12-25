#pragma once
#include<string>
#include<vector>

#include"Math/Vector3.h"

#include"Enemy.h"

struct PopData {
	std::string areaName;//出現エリアの名前
	Vector3 areaPosition;//出現エリアの場所
	float popAreaSize;//出現エリアのサイズ
	float PopStartingArea;//出現処理をする範囲
	float PopInterval;//出現間隔
	float PopIntervalCount = 0;	
	float maxAreaPop;//エリアでの最大出現数
	float maxAreaPopCount=0;
};




class EnemyPopManager {

public:
	/// <summary>
	/// jsonファイル読み込み
	/// </summary>
	void LoadPopdata();

	void Update();

	std::unique_ptr<Enemy> PopEnemy();

	//プレイヤーの座標取得
	void SetPlayerWorld(const WorldTransform* world) { playerWorld_ = world; }

private:

#pragma region 読み込み用各種変数
	//ファイルのフルパス
	std::string filePath = "resources/enemyPopData.json";

	//最大エリア数
	static const int areaNum = 1;

	//エリアの名前
	std::string popAreaName[areaNum] = {
		"PopArea1"
	};

	//各アイテムのEnumClass
	enum class INE {
		POSITION,
		POPAREASIZE,
		RANGEPOPSTART,
		POPINTERVAL,
		MAXAREAPOP
	};

	//各アイテム名のstring配列
	std::string itemNames[5] = {
		"position",
		"PopAreaSize",
		"RangeWithPopStarting",
		"PopInterval",
		"maxAreaPopCount"
	};
#pragma endregion

	//出現データまとめ
	std::vector<PopData>popDatas_;

	//プレイヤーの座標
	const  WorldTransform* playerWorld_=nullptr;



};

