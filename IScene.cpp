#include "IScene.h"


int IScene::sceneNo = 0;

IScene::~IScene() {}

int IScene::GetSceneNo() { return sceneNo; }