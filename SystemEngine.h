#pragma once
#include<Windows.h>
#include<cstdint>
#include <string>
#include<format>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#include<dxgidebug.h>
#pragma comment(lib,"dxguid.lib")
#include<dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include"Matrix.h"
#include"externals/imgui/imgui.h"
#include"externals/Imgui/imgui_impl_dx12.h"
#include"externals/Imgui/imgui_impl_win32.h"
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"
#include<vector>
#include<fstream>
#include<sstream>
#include<wrl.h>

#define _USE_MATH_DEFINES
#include<math.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

struct Vector2 {
	float x;
	float y;
};

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	int32_t enableTexture;
	float padding2[3];
};

struct WorldTransformation {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};


class SystemEngine {
public:



	/// <summary>
	/// ウィンドウなどの初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// システム更新処理
	/// </summary>
	void Update();

/// <summary>
/// 描画
/// </summary>
	void Draw();
private:
#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
#endif // _DEBUG
	HWND hwnd;
	HRESULT hr;
	ID3D12Device* device = nullptr;
	//コマンドキューを生成
	ID3D12CommandQueue* commandQueue = nullptr;
	//コマンドアロケータを生成する
	ID3D12CommandAllocator* commandAllocator = nullptr;
	//コマンドリストを生成
	ID3D12GraphicsCommandList* commandList = nullptr;
	//スワップチェーンを生成する
	IDXGISwapChain4* swapChain = nullptr;
	//
	ID3D12Resource* swapChainResources[2] = { nullptr };
	//
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//フェンス
	ID3D12Fence* fence = nullptr;
	uint32_t fenceValue = 0;
	//FenceのSignalを持つためのイベントを作成する
	HANDLE fenceEvent;
};