#pragma once

#include<string>
#include<d3d12.h>
#include<dxcapi.h>

class GraphicsSystem
{
public://静的メンバ
	static IDxcBlob* CompileShader(
		//CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		//Compilerに使用するProfire
		const wchar_t* profile,
		//初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);

public:
	void Initialize(ID3D12Device* device);


	void PreDraw(ID3D12GraphicsCommandList* commandList);
private:

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature = nullptr;

	//グラフィックパイプライン
	ID3D12PipelineState* graphicsPipelineState = nullptr;
};

