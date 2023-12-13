#pragma once
#include<string>
#include<vector>

#include<array>

#include"externals/DirectXTex/d3dx12.h"

#include"externals/DirectXTex/DirectXTex.h"
#include"DirectXFunc.h"

#include<wrl.h>


class TextureManager {
public://シングルトンパターン
	static TextureManager *GetInstance();

private:
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager& o) = delete;
	const TextureManager& operator=(const TextureManager& o) = delete;

public:

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// 仮利用の画像
	/// </summary>
	static int uvChecker_;


	/// <summary>
	/// 画像の読み込み処理
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	static int LoadTex(const std::string& filePath);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="DXF_"></param>
	void Initialize(DirectXFunc* DXF_);
	
	void Finalize();
		

private://メンバ関数
	/// <summary>
	/// データを作成
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <param name="mipImages"></param>
	/// <returns></returns>
	int CreateData(const std::string& filePath,const DirectX::ScratchImage& mipImages);

	bool CheckSameData(const std::string& filepath);

	int GetDataFromPath(const std::string& path);
private://メンバ変数

	//
	DirectXFunc* DXF;

	
	struct Texturedata{
		int texManagementNumber;
		std::string filePath;
	};

	std::vector<Texturedata*>datas_;

	
	//const size_t maxTexNum_ = 256;
};