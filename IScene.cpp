#include "IScene.h"

//初期シーンの設定
int IScene::sceneNo = STAGE;

IScene::~IScene() {}

int IScene::GetSceneNo() { return sceneNo; }