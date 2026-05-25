module;
#include <string>
#include <string_view>
#include <Graphics/d3dx12.h>
#include <wrl/client.h>

#include <DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")
module TextureManager;

import Texture;

using Microsoft::WRL::ComPtr;
using namespace DirectX;

size_t AlignmentedSize(size_t size, size_t alignment) {
	return size + alignment - size % alignment;
}

Umi::TextureID Umi::TextureManager::LoadTexture(std::string_view filePath)
{
	TextureData textureData;

	TexMetadata metadata = {};
	ScratchImage scratchImg = {};

	for (auto& tex : textureList) 
	{
		if (tex.filePath == filePath) {
			return TextureID{ static_cast<uint32_t>(&tex - &textureList[0]) };
		}
	}

	auto result = LoadFromWICFile(
		std::wstring(filePath.begin(), filePath.end()).c_str(), WIC_FLAGS_NONE,
		&metadata, scratchImg);
	textureData.filePath = std::string(filePath);

	auto img = scratchImg.GetImage(0, 0, 0);

	D3D12_HEAP_PROPERTIES uploadHeapProp = {};
	uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	uploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	uploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	uploadHeapProp.CreationNodeMask = 0;
	uploadHeapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * img->height;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;

	ID3D12Resource* uploadBuff = nullptr;

	result = graphicsContext.device->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadBuff));


	D3D12_HEAP_PROPERTIES texHeapProp = {};
	texHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	texHeapProp.CreationNodeMask = 0;
	texHeapProp.VisibleNodeMask = 0;

	resDesc.Format = metadata.format;
	resDesc.Width = metadata.width;
	resDesc.Height = metadata.height;
	resDesc.DepthOrArraySize = metadata.arraySize;
	resDesc.MipLevels = metadata.mipLevels;
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	result = graphicsContext.device->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&textureData.texBuff));

	uint8_t* mapForImg = nullptr;
	result = uploadBuff->Map(0, nullptr, (void**)&mapForImg);
	auto srcAddress = img->pixels;
	auto rowPitch = AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
	for (int y = 0; y < img->height; ++y) {
		std::copy_n(srcAddress,
			rowPitch,
			mapForImg);//コピー
		//1行ごとの辻褄を合わせてやる
		srcAddress += img->rowPitch;
		mapForImg += rowPitch;
	}

	uploadBuff->Unmap(0, nullptr);

	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource = uploadBuff;
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Offset = 0;
	src.PlacedFootprint.Footprint.Width = metadata.width;
	src.PlacedFootprint.Footprint.Height = metadata.height;
	src.PlacedFootprint.Footprint.Depth = metadata.depth;
	src.PlacedFootprint.Footprint.RowPitch = AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
	src.PlacedFootprint.Footprint.Format = img->format;

	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource = textureData.texBuff.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	ID3D12Fence* fence = nullptr;
	UINT64 fenceValue = 0;
	result = graphicsContext.device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	//_cmdAllocator->Reset();//キューをクリア
	//commandList->Reset(_cmdAllocator, nullptr);

	graphicsContext.commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.Transition.pResource = textureData.texBuff.Get();
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	graphicsContext.commandList->ResourceBarrier(1, &BarrierDesc);
	graphicsContext.commandList->Close();

	//コマンドリストの実行
	ID3D12CommandList* cmdlists[] = { graphicsContext.commandList };
	graphicsContext.commandQueue->ExecuteCommandLists(1, cmdlists);
	////待ち
	graphicsContext.commandQueue->Signal(fence, ++fenceValue);

	if (fence->GetCompletedValue() != fenceValue) {
		auto event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceValue, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
	graphicsContext.commandAllocator->Reset();//キューをクリア
	graphicsContext.commandList->Reset(graphicsContext.commandAllocator, nullptr);

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	descHeapDesc.NodeMask = 0;//マスクは0
	descHeapDesc.NumDescriptors = 1;//ビューは今のところ１つだけ
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//シェーダリソースビュー(および定数、UAVも)
	result = graphicsContext.device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&textureData.texDescHeap));//生成

	//通常テクスチャビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = metadata.format;//DXGI_FORMAT_R8G8B8A8_UNORM;//RGBA(0.0f～1.0fに正規化)
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;//後述
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;//ミップマップは使用しないので1

	graphicsContext.device->CreateShaderResourceView(textureData.texBuff.Get(), //ビューと関連付けるバッファ
		&srvDesc, //先ほど設定したテクスチャ設定情報
		textureData.texDescHeap->GetCPUDescriptorHandleForHeapStart()//ヒープのどこに割り当てるか
	);


	textureList.push_back(textureData);

	if (uploadBuff) {
		uploadBuff->Release();
	}

	return TextureID{ static_cast<uint32_t>(textureList.size() - 1) };
}

void Umi::TextureManager::UnloadTexture(TextureID id)
{
	//auto& texData = textureList[static_cast<uint32_t>(id)];
	//texData.texBuff->Release();
}

Umi::TextureData& Umi::TextureManager::GetTextureData(TextureID id)
{
	return textureList[static_cast<uint32_t>(id)];
}
