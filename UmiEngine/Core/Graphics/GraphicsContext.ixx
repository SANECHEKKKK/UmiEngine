module;
#include <Graphics/d3dx12.h>
export module GraphicsContext;

export namespace Umi
{
	struct GraphicsContext
	{
		ID3D12Device* device{ nullptr };
		ID3D12CommandAllocator* commandAllocator{ nullptr };
		ID3D12GraphicsCommandList* commandList{ nullptr };
		ID3D12CommandQueue* commandQueue{ nullptr };
		GraphicsContext() = default;
		GraphicsContext(ID3D12Device* device, ID3D12CommandAllocator* commandAllocator, ID3D12GraphicsCommandList* commandList, ID3D12CommandQueue* commandQueue) 
			: device(device), commandAllocator(commandAllocator), commandList(commandList), commandQueue(commandQueue) {}
	};
}