#include"SceneManager.h"

#include<imgui.h>


#pragma region シーンのh
#include"GameScene.h"
#pragma endregion

//シーン
enum SCENE {
	STAGE,
	SceneCount	//使うな
};

void SceneManager::Initialize() {

	///初期シーン設定
	currentSceneNo_ = STAGE;

	//シーンの数取得
	sceneArr_.resize((size_t)SCENE::SceneCount);
	
	//各シーンの情報設定
	sceneArr_[STAGE] = std::make_unique<GameScene>();
	
	/*
	sceneArr_[TITLE] = std::make_unique<TitleScene>();
	sceneArr_[STAGE] = std::make_unique<PlayScene>();
	sceneArr_[CLEAR] = std::make_unique<ClearScene>();
	*/
}

void SceneManager::Update() {

	//デバッグ表示
	DebugWindow();

	//シーンチェック
	prevSceneNo_ = currentSceneNo_;
	currentSceneNo_ = sceneArr_[currentSceneNo_]->GetSceneNo();

	//シーン変更チェック
	if (prevSceneNo_ != currentSceneNo_) {
		//変更していたら		
		//初期化処理
		sceneArr_[currentSceneNo_]->Initialize();
	}

	//シーン更新処理
	sceneArr_[currentSceneNo_]->Update();
}

void SceneManager::Draw() {
	//描画処理
	sceneArr_[currentSceneNo_]->Draw();
}

void SceneManager::EndFrame() {

}

void SceneManager::Finalize() {

}

void SceneManager::DebugWindow() {
#ifdef _DEBUG
	if (!deleteWindow) {
		ImGui::Begin("SceneManager");
		ImGui::Text("SceneNo.%d", currentSceneNo_);
		ImGui::Checkbox("deleteWindow", &deleteWindow);
		ImGui::End();
	}
#endif // _DEBUG

}

SceneManager* SceneManager::GetInstance() {
	static SceneManager ins;
	return &ins;
}