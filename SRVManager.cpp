#include "SRVManager.h"
#include"function.h"

SRVManager* SRVManager::GetInstance() {
	static SRVManager instance;
	return &instance;
}

int SRVManager::CreateSRV(ID3D12Resource* textureResource, ID3D12Resource* intermediateResource,D3D12_SHADER_RESOURCE_VIEW_DESC& srvdesc) {
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPU_DES_HANDLE();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPU_DES_HANDLE();

	//srvの生成
	DXF_->GetDevice()->CreateShaderResourceView(PushTextureResource(textureResource), &srvdesc, textureSrvHandleCPU);


	if (intermediateResource!=nullptr) {
		DXF_->KickCommand();
		intermediateResource->Release();
	}

	return AddtextureNum(textureSrvHandleGPU);
}

void SRVManager::Initialize(DirectXFunc* DXF) {

	DXF_ = DXF;

	//SRV用のヒープでディスクリプタの数は１２８。SRVはSHADER内で触るものなので、ShaderVisibleはtrue
	srvDescriptorHeap = CreateDescriptorHeap(DXF_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	descriptorSizeSRV = DXF_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void SRVManager::Finalize() {
	
	for (ID3D12Resource* texRsource : textureResources_) {
		texRsource->Release();
		texRsource = nullptr;
	}

	srvDescriptorHeap->Release();

}

ID3D12Resource* SRVManager::PushTextureResource(ID3D12Resource* resource) {
	textureResources_.emplace_back(std::move(resource));
	size_t num = textureResources_.size() - 1;
	return textureResources_[num];
}

D3D12_CPU_DESCRIPTOR_HANDLE SRVManager::GetCPU_DES_HANDLE() {
	return GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, GetDataSize() + 1);
}

D3D12_GPU_DESCRIPTOR_HANDLE SRVManager::GetGPU_DES_HANDLE() {
	return  GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, GetDataSize() + 1);
}
