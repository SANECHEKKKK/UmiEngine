module;
#include <Graphics/d3dx12.h>
export module GraphicsContext;

import DescriptorHeap;

export namespace Umi
{
	struct GraphicsContext
	{
		ID3D12Device* device{ nullptr };
		ID3D12CommandAllocator* commandAllocator{ nullptr };
		ID3D12GraphicsCommandList* commandList{ nullptr };
		ID3D12CommandQueue* commandQueue{ nullptr };
		
		DescriptorHeap& descriptorHeap2D;
		DescriptorHeap& descriptorHeap3D;

		GraphicsContext(DescriptorHeap& descriptorHeap2D, DescriptorHeap& descriptorHeap3D) : descriptorHeap2D(descriptorHeap2D), descriptorHeap3D(descriptorHeap3D) {}
		GraphicsContext(ID3D12Device* device, ID3D12CommandAllocator* commandAllocator, ID3D12GraphicsCommandList* commandList, ID3D12CommandQueue* commandQueue, DescriptorHeap& descriptorHeap2D, DescriptorHeap& descriptorHeap3D)
			: device(device), commandAllocator(commandAllocator), commandList(commandList), commandQueue(commandQueue), descriptorHeap2D(descriptorHeap2D), descriptorHeap3D(descriptorHeap3D) {}
	};
}