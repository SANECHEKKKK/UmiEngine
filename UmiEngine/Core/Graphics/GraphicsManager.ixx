module;
#include <Graphics/d3dx12.h>
#include <dxgi1_6.h>
#include <vector>

export module GraphicsManager;

namespace Umi
{
	export class GraphicsManager
	{
	private:
		ID3D12Device* device = nullptr;
		IDXGIFactory6* factory = nullptr;

		//-----------FEATURE_LEVEL-----------
		const D3D_FEATURE_LEVEL fls[4] =
		{
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};
		D3D_FEATURE_LEVEL featureLevel;
		//-----------------------------------


		//-----------COMMAND_LIST------------
		ID3D12CommandAllocator* commandAllocator = nullptr;
		ID3D12GraphicsCommandList* commandList = nullptr;
		ID3D12CommandQueue* commandQueue = nullptr;
		//-----------------------------------

		//-------------SWAP_CHAIN-------------
		IDXGISwapChain4* swapChain = nullptr;
		std::vector<ID3D12Resource*> backBuffers;
		//-----------------------------------

		//-------------DESCRIPTOR_HEAP-------------
		ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
		//-----------------------------------------

	public:
		GraphicsManager(HWND hwnd);
	};
}
