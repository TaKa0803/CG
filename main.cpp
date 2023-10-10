#include"DirectXFunc.h"
#include"WinApp.h"
#include"ImGuiManager.h"
#include"Graphics.h"
#include"TextureManager.h"


#include"Model.h"
#include"Sprite.h"


#include<dxgidebug.h>




#pragma region 構造体
struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {
		//リソースリークチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1>debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			//警告時に止まる
		}
	}
};
#pragma endregion



//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	HRESULT hr= CoInitializeEx(0, COINIT_MULTITHREADED);
	
	D3DResourceLeakChecker lackCheck;

#pragma region 基板初期化
	WinApp* winApp = WinApp::GetInstance();
	winApp->Initialize();

	DirectXFunc* DXF = DirectXFunc::GetInstance();
	DXF->Initialize(winApp);

	GraphicsSystem *graphics = nullptr;
	graphics = new GraphicsSystem;
	graphics->Initialize(DXF->GetDevice());


	ImGuiManager* imguiManager = nullptr;
	imguiManager = new ImGuiManager;
	imguiManager->Initialize(winApp, DXF);
#pragma endregion


	Sprite *sprite = new Sprite();
	sprite->Initialize(DXF);

	Model* model = nullptr;
	model = Model::CreateSphere(128,true);


	Model* Mteapot = nullptr;
	Mteapot = Model::CreateFromOBJ("teapot.obj");

	Model* Mbunny = nullptr;
	Mbunny = Model::CreateFromOBJ("bunny.obj");



int texture = TextureManager::LoadTex("resources/uvChecker.png");

int monstarBall = TextureManager::LoadTex("resources/monsterBall.png");

//DirectX::ScratchImage mipImages = LoadTexture(modeldata.material.textureFilePath);
	


	

#pragma region 更新

#pragma region メイン
	//カメラ情報
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };
	//Transform変数を作る
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//Transform変数を作る
	Transform OBJT{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Transform teapot{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };


	//リソースのtransform
	Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Transform UVT{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	

	bool useMonsterBall = true;
	bool useTexture = true;

	enum Mode {
		Triangle,
		Sphere,
		Sprite,
		Model,
	};

	Mode mode = Model;

	int num = Triangle;
#pragma endregion

	while (winApp->ProcessMessage())
	{	
		imguiManager->PreUpdate();
		//ゲーム内処理
#pragma region 更新処理
		//開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に書き換える
		ImGui::ShowDemoWindow();
#pragma region 全部の更新処理とCBuffer転送

			ImGui::Begin("Camera");
			ImGui::DragFloat3("Camera translate", &cameraTransform.translate.x, 0.01f);
			ImGui::DragFloat3("Camera rotate", &cameraTransform.rotate.x, 0.01f);
			ImGui::Checkbox("useTexture", &useTexture);
			ImGui::Checkbox("useMonsterBall", &useMonsterBall);
			ImGui::DragInt("scene", &num, 1, 0, 3);

			ImGui::End();
			if (useTexture) {
				Mteapot->IsEnableTexture(true);
				Mbunny->IsEnableTexture(true);
				sprite->IsEnableTexture(true);
				
			}
			else {
				Mteapot->IsEnableTexture(false);
				Mbunny->IsEnableTexture(false);
				sprite->IsEnableTexture(false);

			}

			//カメラ処理
			Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrix = Inverse(cameraMatrix);
			//透視投影行列
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
			Matrix4x4 VP =Multiply(viewMatrix, projectionMatrix);

			
#pragma region UV
			ImGui::Begin("UV");
			ImGui::DragFloat2("trans", &UVT.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("scale", &UVT.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("rotate", &UVT.rotate.z);
			ImGui::End();
			Matrix4x4 worldUV = MakeAffineMatrix(UVT.scale, UVT.rotate, UVT.translate);

			//materialData->uvTransform = worldUV;

			sprite->SetUV_T(worldUV);
			Mteapot->SetUV(worldUV);
#pragma endregion
			ImGui::Begin("obj");
			ImGui::DragFloat3("pos", &teapot.translate.x, 0.01f);
			ImGui::DragFloat3("bunny pos", &OBJT.translate.x, 0.01f);
			ImGui::End();

#pragma endregion
#pragma endregion

		imguiManager->PostUpdate();
#pragma region 描画
			
#pragma region コマンドを積み込んで確定させる
		DXF->PreDraw();
		imguiManager->PreDraw();
		graphics->PreDraw(DXF->GetCMDList());
		
		
#pragma region 描画コマンド
			switch (num)
			{
			case Sphere:
				Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
				
				model->Draw(worldMatrix,VP, monstarBall);				
			
				break;
			case Sprite:

				Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
	
				sprite->Draw(worldMatrixSprite, texture);			

				break;
			case Model:
				
				Matrix4x4 Wtea = MakeAffineMatrix(teapot.scale, teapot.rotate, teapot.translate);
				
				Matrix4x4 Wobj = MakeAffineMatrix(OBJT.scale, OBJT.rotate, OBJT.translate);

				Mteapot->Draw(Wtea,VP,texture);
				
				Mteapot->Draw(Wobj, VP, texture);
				//Mbunny->Draw(Wobj, VP, texture);

				break;
			default:
				break;
			}

			
			//コマンドを組むおわり
#pragma endregion
			

		imguiManager->PostDraw();
		DXF->PostDraw();
#pragma endregion
#pragma endregion
		
	}
#pragma endregion



#pragma region ReportLiveObjects
	lackCheck.~D3DResourceLeakChecker();
#pragma endregion

	imguiManager->Finalize();	
	DXF->Finalize();	
	winApp->Finalize();


	delete imguiManager;
	

	CoUninitialize();

	return 0;
}