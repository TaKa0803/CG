#pragma once
#include"Scenes.h"


//基底クラス
class IScene {
protected:
	//シーン番号
	static int sceneNo;

public:

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual ~IScene();

	/// <summary>
	/// シーン番号取得
	/// </summary>
	/// <returns>シーン番号</returns>
	int GetSceneNo();

};