#include"SystemEngine.h"

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



MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	//1中で必要になる変数の宣言
	MaterialData materialdata;
	std::string line;
	//２ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());
	//３実際にファイルを読みまてりあｌDataを構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialdata.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	//４MaterialDataを返す
	return materialdata;

}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
#pragma region 中で必要となる変数の宣言
	ModelData modeldata;//構築するModelData
	std::vector<Vector4> positions;//位置
	std::vector<Vector3>normals;//法線
	std::vector<Vector2>texcoords;//texture座標
	std::string line;//ファイルからよんだ一行を格納するもの	
#pragma endregion
#pragma region ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());//開けなかったら止める
#pragma endregion
#pragma region 実際にファイルを読みModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;//先頭の識別子を読む

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			position.x *= -1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			//texcoord.x = 1.0f - texcoord.x;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			//面は三角形限定その他は未対応
			VertexData triangle[3];
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは位置//UV/法線で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];

				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');//区切りでインデクスを読んでいく
					elementIndices[element] = std::stoi(index);


				}
				//要素へのIndexから、実際の要素の値を取得して頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				//VertexData vertex = { position,texcoord,normal };
				//modeldata.vertices.push_back(vertex);

				triangle[faceVertex] = { position,texcoord,normal };
			}
			modeldata.vertices.push_back(triangle[2]);
			modeldata.vertices.push_back(triangle[1]);
			modeldata.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を変更する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一改装にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modeldata.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}

		
	}
#pragma endregion
#pragma region 4.ModelDataを返す
	return modeldata;
#pragma endregion
}

#pragma region Log関連
std::wstring ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

void Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

#pragma endregion


IDxcBlob* CompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfire
	const wchar_t*profile,
	//初期化で生成したものを3つ
	IDxcUtils*dxcUtils,
	IDxcCompiler3*dxcCompiler,
	IDxcIncludeHandler*includeHandler)
{
#pragma region //1.hlslファイルを読む
	//これからシェーダーをコンパイルするとログに出す
	Log(ConvertString(std::format(L"Begin CompileShader , path:{}, profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8の文字コードであることを感知
#pragma endregion
#pragma region //2.Compileする
	LPCWSTR arguments[] = {			
		filePath.c_str(),			//コンパイル対象のhlslのファイル名
		L"-E",L"main",				//エントリーパイントの指定。個本的にmain以外にしない
		L"-T",profile,				//ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",	//デバッグ用の情報を埋め込む
		L"-Od",						//最適化を外しておく
		L"-Zpr",					//メモリアウトは行優先
	};
	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,		//読み込んだファイル
		arguments,					//コンパイルオプション
		_countof(arguments),		//コンパイルオプションの数
		includeHandler,				//includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)	//コンパイル結果
	);
	//致命的な状況02_00
	assert(SUCCEEDED(hr));
#pragma endregion
#pragma region //3.警告・エラーが出ていないか確認する
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		//警告・エラーダメ絶対
		assert(false);
	}
#pragma endregion
#pragma region //4.Compile結果を受け取って返す
		//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Complete Succeeded path:{}, profile:{}\n", filePath, profile)));
	//もう使わないリソースを開放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
#pragma endregion
}

//ウィンドウプロシーシャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	//メッセージに対してゲーム固有の処理をする
	switch (msg)
	{
	case WM_DESTROY:PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
	
#pragma region VertexResourceを生成する
	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResorceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResorceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResorceDesc.Width = sizeInBytes;
	//バッファの場合はこれらは１にする決まり
	vertexResorceDesc.Height = 1;
	vertexResorceDesc.DepthOrArraySize = 1;
	vertexResorceDesc.MipLevels = 1;
	vertexResorceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	vertexResorceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
#pragma endregion
	ID3D12Resource* vertexResource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResorceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));
	return vertexResource;
}

ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));

	return descriptorHeap;
}

//Textureデータを読む
DirectX::ScratchImage LoadTexture(const std::string& filePath) {
	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//みっぷマップ作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//みっぷマップ月のデータを返す
	return mipImages;
}

//DirectX12のTextureResourceを作る
ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
#pragma region 1.metadataをもとにResourceの設定
	//metadataをもとにResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);									//Textureの幅
	resourceDesc.Height = UINT(metadata.height);								//Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);						//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);					//奥行き　or 配列Textureの配列数
	resourceDesc.Format = metadata.format;										//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;											//サンプリングカウント１固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);		//Textureの次元数。普段使っているのは二次元
#pragma endregion
#pragma region 2.利用するHeapの設定
	//利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//細かい設定を行う
	
#pragma endregion
#pragma region 3.Resourceを生成する
	//Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,							//Heapの設定
		D3D12_HEAP_FLAG_NONE,						//Heapの特殊な設定。特になし。
		&resourceDesc,								//Reosurceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,			//初回のResourceState。Textureは基本読むだけ
		nullptr,									//Clear最適地。使わないのでnullptr
		IID_PPV_ARGS(&resource));					//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	
#pragma endregion
	return resource;
}


[[nodiscard]]
ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages,
	ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ID3D12Resource* intermediateResource = CreateBufferResource(device, intermediateSize);
	UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());
	//
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;
}



ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {
#pragma region 生成するReosurceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;											//Textureの幅
	resourceDesc.Height = height;										//Textureの高さ
	resourceDesc.MipLevels = 1;											//mipmapの数
	resourceDesc.DepthOrArraySize = 1;									//奥行き　or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;				//DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;									//サンプリングカウント、１固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;		//２次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;		//DepthStencilとして使う通知

	//りようするHeapの設定
	D3D12_HEAP_PROPERTIES heapProperities{};
	heapProperities.Type = D3D12_HEAP_TYPE_DEFAULT;						//VRAM上に作る
#pragma region 深度地クリア最適化設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//1.0（最大値）でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//フォーマット、Resourceと合わせる
#pragma endregion
#pragma region Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperities,												//Heapの設定
		D3D12_HEAP_FLAG_NONE,											//Heapの特殊な設定、特になし
		&resourceDesc,													//Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,								//深度値を書き込む状態にしておく
		&depthClearValue,												//Clear最適地
		IID_PPV_ARGS(&resource));										//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	return resource;
#pragma endregion

#pragma endregion

}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}
D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}


//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	CoInitializeEx(0, COINIT_MULTITHREADED);
	D3DResourceLeakChecker lackCheck;
#pragma region ウィンドウ生成 
	WNDCLASS wc{};
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = L"A";
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&wc);

	//
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;

	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd = CreateWindow(
		wc.lpszClassName,
		L"CG2",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr);
	
#pragma region DebugLayer
#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバッグレイヤーを有効化
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif
#pragma endregion
	

	ShowWindow(hwnd, SW_SHOW);
#pragma endregion
#pragma region DXGIFactoryの生成
	//DXGIファクトリーの生成
	IDXGIFactory7* dxgiFactory = nullptr;
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr));
#pragma endregion
#pragma region 使用するアダプタを決定する
	//使用するアダプタ用の変数。最初にnullptrを入れておく
	IDXGIAdapter4* useAdapter = nullptr;
	//いい順にアダプタを積む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
		//アダプターの情報取得
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));
		//ソフトウェアアダプタでなければ採用！
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//ログに出力
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;
	}
	//見つからなければ起動できない
	assert(useAdapter != nullptr);
#pragma endregion
#pragma region D3D12Deviceの生成
	ID3D12Device* device = nullptr;
	//機能レベルとログと出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0 };
	const char* featurelevelString[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
		if (SUCCEEDED(hr)) {
			Log(std::format("FeatureLevel : {}\n", featurelevelString[i]));
			break;
		}
	}
	//デバイスの生成がうまく行かなかったので起動できない
	assert(device != nullptr);
	Log("Complete create D3D12Device!!\n");	
#pragma region エラー・警告で停止
#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
#pragma region エラー警告の抑制
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11
			//https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
#pragma endregion
		//開放
		infoQueue->Release();
	}
#endif
#pragma endregion
#pragma endregion
#pragma region コマンドキューの生成
	//コマンドキューを生成
	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	//コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
#pragma endregion
#pragma region CommandListを生成
	//コマンドアロケータを生成する
	ID3D12CommandAllocator* commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(hr));

	//コマンドリストを生成
	ID3D12GraphicsCommandList* commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(hr));
#pragma endregion
#pragma region SwapChainを生成する
	//スワップチェーンを生成する
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = kClientWidth;
	swapChainDesc.Height = kClientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
	assert(SUCCEEDED(hr));
#pragma endregion
#pragma region DescriptorHeap
	//RTVようのヒープでディスクリプタの数は２。RTVはSHADER内で触るものではないのでShaderVisibleはfalse
	ID3D12DescriptorHeap* rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	//SRV用のヒープでディスクリプタの数は１２８。SRVはSHADER内で触るものなので、ShaderVisibleはtrue
	ID3D12DescriptorHeap* srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	//DSV用のヒープでディスクリプタの数は１。DSVはShader内で触るものではないので、ShaderVisibleはfalse
	ID3D12DescriptorHeap* dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1,false);
#pragma endregion
#pragma region DescriptorSize
	const uint32_t descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	const uint32_t descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	const uint32_t descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
#pragma endregion

#pragma region SwapChain
	ID3D12Resource* swapChainResources[2] = { nullptr };
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));
#pragma endregion
#pragma region RTV
	//RTV
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	//ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = GetCPUDescriptorHandle(rtvDescriptorHeap, descriptorSizeRTV, 0);
	//
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//
	rtvHandles[0] = rtvStartHandle;
	device->CreateRenderTargetView(swapChainResources[0],&rtvDesc ,rtvHandles[0]);
	//
	rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);
#pragma endregion
#pragma region DSV
	//DepthStencilTextureをウィンドウサイズで作成
	ID3D12Resource* depthStencilResource = CreateDepthStencilTextureResource(device, kClientWidth, kClientHeight);
	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//Format基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;	//2dTexture
	//DSVHEapの先頭にDSVを作る
	device->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
#pragma endregion
#pragma region FenceとEventの生成と処理
	ID3D12Fence* fence = nullptr; 
	uint32_t fenceValue = 0;
	hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	//FenceのSignalを持つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent!=nullptr);
#pragma endregion
#pragma region PSO群
#pragma region DXCの初期化
	//dxcCompilerを初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//現時点でincludeはしないが、includeに対応するための設定
	IDxcIncludeHandler* includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));
#pragma endregion
#pragma region RootSignatureを生成する
	//RootSignatureの作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
#pragma region RootParameter 
	//RootParameter作成。PixelShaderのMAterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 1;						//レジスタ番号０とバインド

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	//PixelShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;						//レジスタ番号０とバインド
	
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.ShaderRegister = 2;

	
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;								//0から始まる
	descriptorRange[0].NumDescriptors = 1;									//数
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;			//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算
	
	
	//DescriptorTable
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;;		//DescriptorHeapを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					//PixelShaderで使う 
	rootParameters[2].DescriptorTable.pDescriptorRanges=descriptorRange;				//tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges=_countof(descriptorRange);	//tableで利用する
	
	descriptionRootSignature.pParameters = rootParameters;					//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);		//配列の長さ


	
#pragma region Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイニアリング
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
#pragma endregion

#pragma endregion
	//シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリをもとに生成
	ID3D12RootSignature* rootSignature = nullptr;
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));
#pragma endregion
#pragma region InputLayoutの設定
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);
#pragma endregion
#pragma region BlendStateの設定を行う
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
#pragma endregion
#pragma region RasterizeStateの設定を行う
	//RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
#pragma endregion
#pragma region ShaderをCompileする
	//Shaderをコンパイルする
	IDxcBlob* vertexShaderBlob = CompileShader(L"resources/shaders/Object3d.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);

	IDxcBlob* pixelShaderBlob = CompileShader(L"resources/shaders/Object3d.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);
#pragma endregion
#pragma region DepthStencilStateの設定を行う
	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqualつまりちかければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
#pragma endregion
#pragma region PSOを生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature;	//RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;	//InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジのタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//実際に生成
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
#pragma endregion
#pragma endregion

#pragma region Sprite
#pragma region VertexResourceとVertexBufferViewを用意
	//Sprite用の頂点リソースを作る
	ID3D12Resource* vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 4);

	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	//リソース用の先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 4;
	//頂点当たりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	
#pragma endregion
#pragma region 頂点データを設定する
	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	//一枚目の三角形
	vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite[0].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite[1].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	vertexDataSprite[2].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[3].position = { 640.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[3].texcoord = { 1.0f,0.0f };
	vertexDataSprite[3].normal = { 0.0f,0.0f,-1.0f };

	ID3D12Resource* indexResourceSprite = CreateBufferResource(device, sizeof(uint32_t) * 6);

	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	//リソースの先頭アドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス６つ分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはうんち
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	uint32_t *indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0]=1;
	indexDataSprite[1]=3;
	indexDataSprite[2]=0;

	indexDataSprite[3]=3;
	indexDataSprite[4]=2;
	indexDataSprite[5]=0;


#pragma endregion
#pragma region Transform周りを作る
	ID3D12Resource* transformationMatrixResourceSprite = CreateBufferResource(device, sizeof(WorldTransformation));
	//データを書き込む
	WorldTransformation* transformationMatrixDataSprite = nullptr;
	//書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	transformationMatrixDataSprite->WVP = MakeIdentity4x4();
	transformationMatrixDataSprite->World = MakeIdentity4x4();
#pragma endregion
#pragma endregion

/*
#pragma region 円
#pragma region VertexBufferViewを作成

	const float kSubdivision = 128;//分割数
	//頂点数
	int point = (int)kSubdivision * (int)kSubdivision * 6;
	ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * point);


	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//リソースの戦闘のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * point;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);
#pragma endregion
#pragma region Resourceにデータを書き込む
	//時計周りに点を設定していく
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr,
		reinterpret_cast<void**>(&vertexData));
	
	//経度一つ分の角度δ
	const float kLonEvery = (float)M_PI * 2.0f / float(kSubdivision);
	//緯度一つ分の角度Θ
	const float kLatEvery = (float)M_PI / float(kSubdivision);
	//緯度の方向に分割
	for (int latIndex = 0; latIndex < kSubdivision; ++latIndex) {//σ
		float lat = -(float)M_PI / 2.0f + kLatEvery * latIndex;
		//経度のほう
		for (int lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {//Θ
			uint32_t start = ((int)latIndex * (int)kSubdivision + (int)lonIndex) * 6;
			float lon = lonIndex * kLonEvery;
			//一枚目
			//1(a)
			vertexData[start].position = { cos(lat) * cos(lon),sin(lat),  cos(lat) * sin(lon),1.0f };
			vertexData[start].texcoord ={ (lonIndex) / (float)kSubdivision,1.0f - (latIndex) / (float)kSubdivision };
			vertexData[start].normal = { vertexData[start].position.x,vertexData[start].position.y,vertexData[start].position.z };

			//2(b)
			vertexData[start+1].position = { cos(lat + kLatEvery) * cos(lon),sin(lat+kLatEvery), cos(lat + kLatEvery) * sin(lon),1.0f };
			vertexData[start+1].texcoord = { (lonIndex) / (float)kSubdivision,1.0f - (latIndex+1) / (float)kSubdivision };
			vertexData[start+1].normal = { vertexData[start+1].position.x,vertexData[start+1].position.y,vertexData[start+1].position.z };

			//3(c)
			vertexData[start+2].position = { cos(lat) * cos(lon + kLonEvery),sin(lat),  cos(lat) * sin(lon + kLonEvery),1.0f };
			vertexData[start+2].texcoord = { (lonIndex+1) / (float)kSubdivision,1.0f - (latIndex) / (float)kSubdivision };
			vertexData[start+2].normal = { vertexData[start+2].position.x,vertexData[start+2].position.y,vertexData[start+2].position.z };

			//二枚目
			//
			vertexData[start + 3] = vertexData[start + 1];
			//
			vertexData[start + 4].position = { cos(lat + kLatEvery) * cos(lon+kLonEvery),sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon + kLonEvery),1.0f };
			vertexData[start + 4].texcoord = { (lonIndex+1) / (float)kSubdivision,1.0f - (latIndex+1) / (float)kSubdivision };
			vertexData[start+4].normal = { vertexData[start+4].position.x,vertexData[start+4].position.y,vertexData[start+4].position.z };
			//
			vertexData[start + 5] = vertexData[start + 2];
		}
	}
#pragma endregion
	*/

#pragma region obj
	ModelData modeldata = LoadObjFile("resources","multiMesh.obj");
	//頂点リソースを作る
	ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * modeldata.vertices.size());
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modeldata.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modeldata.vertices.data(), sizeof(VertexData)* modeldata.vertices.size());
#pragma endregion


#pragma region TransformationMatrix用のResourceを作る
	//WVP用のリソースを作る。Matrix４ｘ４1つ分のサイズを用意する
	ID3D12Resource* wvpResource = CreateBufferResource(device, sizeof(WorldTransformation));
	//データを書き込む
	WorldTransformation* wvpData = nullptr;
	//書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおくtextureResource
	wvpData->WVP = MakeIdentity4x4();
	wvpData->World = MakeIdentity4x4();
#pragma endregion
#pragma endregion	





#pragma region Material用のResourceを作る
	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	ID3D12Resource* materialResource = CreateBufferResource(device, sizeof(Material));
	//マテリアルにデータを書き込む
	Material* materialData = nullptr;
	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = true;
	materialData->uvTransform = MakeIdentity4x4();
	//materialData->enableHalfLambert = false;
	//Sprite用のマテリアルリソース
	ID3D12Resource* materialSpriteResource = CreateBufferResource(device, sizeof(Material));
	//マテリアルにデータを書き込む
	Material *materialSpriteData = nullptr;
	materialSpriteResource->Map(0, nullptr, reinterpret_cast<void**>(&materialSpriteData));
	materialSpriteData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialSpriteData->enableLighting = false;
	materialSpriteData->uvTransform = MakeIdentity4x4();
	//materialSpriteData->enableHalfLambert = false;
	//ディレクションライトのマテリアルリソース
	ID3D12Resource* directionalLightResource = CreateBufferResource(device, sizeof(DirectionalLight));
	DirectionalLight* directionalLightData = nullptr;
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	directionalLightData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	directionalLightData->direction = Vector3(0.0f,-1.0f,0.0f);
	directionalLightData->intensity = 1.0f;
#pragma endregion
#pragma region textureの読み込み
	//DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
	DirectX::ScratchImage mipImages = LoadTexture(modeldata.material.textureFilePath);

	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ID3D12Resource* textureResource = CreateTextureResource(device, metadata);
	ID3D12Resource* intermediateResource = UploadTextureData(textureResource, mipImages, device, commandList);

	DirectX::ScratchImage mipImages2 = LoadTexture("resources/monsterBall.png");
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	ID3D12Resource* textureResource2 = CreateTextureResource(device, metadata2);
	ID3D12Resource* intermediateResource2 = UploadTextureData(textureResource2, mipImages2, device, commandList);

#pragma endregion
#pragma region ShaderResourceView(SRV)
	//metadataにSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dtexture
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 1);
	//srvの生成
	device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);


	//metadataにSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dtexture
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2= GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);
	//srvの生成
	device->CreateShaderResourceView(textureResource2, &srvDesc2, textureSrvHandleCPU2);

#pragma endregion


#pragma region ViewportとScissor(シザー)
	//ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = kClientWidth;
	viewport.Height = kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//シザー短形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ短形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = kClientHeight;
#pragma endregion
#pragma region ImGuiの初期化
	//ImGuiの初期化。詳細はさして重要ではないので解説は省略
	//こういうもんである
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(device,
		swapChainDesc.BufferCount,
		rtvDesc.Format, 
		srvDescriptorHeap,
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	
#pragma endregion
#pragma region コマンドをキックする
	//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	hr = commandList->Close();
	assert(SUCCEEDED(hr));
	//GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(1, commandLists);
	//GPUとOSに画面の交換を行うよう通知する
	swapChain->Present(1, 0);
#pragma region GPUにSignalをおくる
	//Fenceの値を更新
	fenceValue++;
	//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue->Signal(fence, fenceValue);
	//Fenceの値が指定したSignal値にたどり着いているか確認する
	//GetCompleteValueの初期値はFence作成時に渡した初期値
	if (fence->GetCompletedValue() < fenceValue) {
		//指定したSignalにたどり着いていないので、だどりつくまでイベントを指定する
		fence->SetEventOnCompletion(fenceValue, fenceEvent);
		//イベント待つ
		WaitForSingleObject(fenceEvent, INFINITE);
	}
	//GPUにシグナルを送るおわり
#pragma endregion 
			//次フレーム用のコマンドリストを取得
	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));
	//コマンドをキックする終わり
#pragma endregion
	//GPUに送ったので殺す
	intermediateResource->Release();
	intermediateResource2->Release();

#pragma region 更新
	//Transform変数を作る
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//カメラ情報
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };
	//リソースのtransform
	Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Transform UVT{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	bool useMonsterBall = true;

	MSG msg{};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			//ゲーム内処理
#pragma region フレームの先頭でImGuiにここからフレームが始まる旨を告げる
			
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			
#pragma endregion
#pragma region 更新処理
			//開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に書き換える
			ImGui::ShowDemoWindow();
#pragma region 回転処理
			//transform.rotate.y += 0.03f;

			ImGui::Begin("Camera");
			ImGui::DragFloat3("Camera translate", &cameraTransform.translate.x, 0.01f);
			ImGui::DragFloat3("Camera rotate", &cameraTransform.rotate.x, 0.01f);
			ImGui::Checkbox("useMonsterBall", &useMonsterBall);
			ImGui::End();

			ImGui::Begin("Sphere");
			ImGui::DragFloat3("pos", &transform.translate.x, 0.01f);
			ImGui::DragFloat3("rotate", &transform.rotate.x, 0.01f);
			ImGui::DragFloat3("scale", &transform.scale.x, 0.01f);
			ImGui::End();

			

			ImGui::Begin("directional Light");
			ImGui::DragFloat3("direction", &directionalLightData->direction.x, 0.01f);
			ImGui::DragFloat4("color", &directionalLightData->color.x, 0.01f);
			ImGui::DragFloat("power",&directionalLightData->intensity,0.01f);
			ImGui::End();
			directionalLightData->direction = Normalize(directionalLightData->direction);

			ImGui::Begin("Sprite");
			ImGui::DragFloat2("trans", &UVT.translate.x, 0.01f,-10.0f,10.0f);
			ImGui::DragFloat2("scale", &UVT.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("rotate", &UVT.rotate.z);
			ImGui::End();
			Matrix4x4 worldUV = MakeAffineMatrix(UVT.scale, UVT.rotate, UVT.translate);
			materialSpriteData->uvTransform=worldUV;
			materialData->uvTransform = worldUV;
#pragma endregion
#pragma region //CBufferの中身の変更
			//ワールド
			Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			//カメラ処理
			Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrix = Inverse(cameraMatrix);
			//透視投影行列
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
			//データを転送
			wvpData->WVP = worldViewProjectionMatrix;
			wvpData->World = worldMatrix;
#pragma endregion
#pragma region Spriteデータの処理
			Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			//スプライト用データ
			Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
			Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(kClientWidth), float(kClientHeight), 0.0f, 100.0f);
			Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
			//データ転送
			transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
			transformationMatrixDataSprite->World = worldMatrixSprite;
#pragma endregion
#pragma endregion
#pragma region 描画
			//ImGuiの内部コマンドを生成する
			ImGui::Render();
#pragma region コマンドを積み込んで確定させる
			//これから書き込むバックバッファのインデックスを取得
			UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
#pragma region TransitionBarrierを張る
			//Transitionbarrierの設定
			D3D12_RESOURCE_BARRIER barrier{};
			//今回のバリアはTransition
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			//Noneにしておく
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			//バリアを張る対象のリソース、現在のバックバッファに対して行う
			barrier.Transition.pResource = swapChainResources[backBufferIndex];
			//遷移前（現在）のResourceState
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			//遷移後のResourceState
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			//TransitionBarrierを張る
			commandList->ResourceBarrier(1, &barrier);
#pragma endregion
#pragma region DSVの設定
			//描画先のRTVとDSVを設定する
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetCPUDescriptorHandle(dsvDescriptorHeap, descriptorSizeDSV, 0);
			commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);
#pragma endregion
			//指定した色で画面全体をクリアする
			float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//青っぽい色
			//指定した深度で画面全体をクリアする
			commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
			commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);		
#pragma region ImGuiの処理
			//描画用のDescriptorHeapの設定
			ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap };
			commandList->SetDescriptorHeaps(1, descriptorHeaps);
#pragma endregion
#pragma region 描画コマンド
			commandList->RSSetViewports(1, &viewport);
			commandList->RSSetScissorRects(1, &scissorRect);
			//RootSignatureを設定。PSOに設定しているけど別途設定が必要
			commandList->SetGraphicsRootSignature(rootSignature);
			commandList->SetPipelineState(graphicsPipelineState);
			commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
			//形状を設定、PSOに設定しているものとはまた別、同じものを設定すると考えておけばいい
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		
			//wvp用のCBufferの場所の設定
			commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
			//マテリアルCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			//
			commandList->SetGraphicsRootConstantBufferView(3,directionalLightResource->GetGPUVirtualAddress() );
			//SRVのDescriptorTableの先頭を設定。２はParameter[2]である。
			commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall? textureSrvHandleGPU2: textureSrvHandleGPU);
			//描画！
			//commandList->DrawInstanced(point, 1, 0, 0);
			commandList->DrawInstanced(UINT(modeldata.vertices.size()), 1, 0, 0);
#pragma region 2D描画コマンド
			//Spriteの描画
			commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);	//VBVを設定
			commandList->IASetIndexBuffer(&indexBufferViewSprite);//IBVを設定
			//マテリアルCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialSpriteResource->GetGPUVirtualAddress());
			//TransformationMatrixCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
			//
			commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
			//描画！！（DrawCall
			commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
#pragma endregion

			//実際のcommandListのImGuiの描画コマンドを積む
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
			//コマンドを組むおわり
#pragma endregion		
#pragma region 画面表示できるようにする
			//画面に描く処理はすべて終わり、画面に移すので状態を遷移
			//今回はRenderTargetからPresentにする
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			//TransitionBarrierを張る
			commandList->ResourceBarrier(1, &barrier);
			//画面表示できるようにするおわり
#pragma endregion	
#pragma endregion
#pragma region コマンドをキックする
				//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
			hr = commandList->Close();
			assert(SUCCEEDED(hr));
			//GPUにコマンドリストの実行を行わせる
			ID3D12CommandList* commandLists[] = { commandList };
			commandQueue->ExecuteCommandLists(1, commandLists);
			//GPUとOSに画面の交換を行うよう通知する
			swapChain->Present(1, 0);
#pragma region GPUにSignalをおくる
			//Fenceの値を更新
			fenceValue++;
			//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
			commandQueue->Signal(fence, fenceValue);
			//Fenceの値が指定したSignal値にたどり着いているか確認する
			//GetCompleteValueの初期値はFence作成時に渡した初期値
			if (fence->GetCompletedValue() < fenceValue) {
				//指定したSignalにたどり着いていないので、だどりつくまでイベントを指定する
				fence->SetEventOnCompletion(fenceValue, fenceEvent);
				//イベント待つ
				WaitForSingleObject(fenceEvent, INFINITE);
			}
			//GPUにシグナルを送るおわり
#pragma endregion 
			//次フレーム用のコマンドリストを取得
			hr = commandAllocator->Reset();
			assert(SUCCEEDED(hr));
			hr = commandList->Reset(commandAllocator, nullptr);
			assert(SUCCEEDED(hr));
			//コマンドをキックする終わり
#pragma endregion
#pragma endregion
		}
	}
#pragma endregion
#pragma region 開放処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	vertexResourceSprite->Release();
	transformationMatrixResourceSprite->Release();
	//生成と逆順に飲む
	wvpResource->Release();
	textureResource->Release();
	textureResource2->Release();
	
	depthStencilResource->Release();
#pragma region 02_00
	//02_01
	indexResourceSprite->Release();
	directionalLightResource->Release();
	materialSpriteResource->Release();
	materialResource->Release();
	vertexResource->Release();
	graphicsPipelineState->Release();
	signatureBlob->Release();
	if (errorBlob) {
		errorBlob->Release();
	}
	rootSignature->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();
#pragma endregion
	CloseHandle(fenceEvent);
	fence->Release();
	dsvDescriptorHeap->Release();
	srvDescriptorHeap->Release();
	rtvDescriptorHeap->Release();
	swapChainResources[0]->Release();
	swapChainResources[1]->Release();
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	device->Release();
	useAdapter->Release();
	dxgiFactory->Release();
#ifdef _DEBUG
	debugController->Release();
#endif // _DEBUG
	CloseWindow(hwnd);
#pragma endregion
#pragma region ReportLiveObjects
	lackCheck.~D3DResourceLeakChecker();
#pragma endregion

	CoUninitialize();

	return 0;
}