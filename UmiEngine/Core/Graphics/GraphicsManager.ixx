module;
#include <Graphics/d3dx12.h>

#include <dxgi1_6.h>
#include <vector>
#include <numeric>

#include <wrl/client.h>
export module GraphicsManager;

using Microsoft::WRL::ComPtr;

export namespace Umi
{
	struct DescriptorHeapAllocator {
		ID3D12DescriptorHeap* heap = nullptr;
		D3D12_DESCRIPTOR_HEAP_TYPE  type{};
		D3D12_CPU_DESCRIPTOR_HANDLE cpuStart{};
		D3D12_GPU_DESCRIPTOR_HANDLE gpuStart{};
		uint32_t                    descriptorSize = 0;
		uint32_t                    capacity = 0;
		std::vector<uint32_t>       freeSlots;

		void Init(ID3D12Device* device, ID3D12DescriptorHeap* h, uint32_t cap) {
			heap = h;
			type = h->GetDesc().Type;
			cpuStart = h->GetCPUDescriptorHandleForHeapStart();
			gpuStart = h->GetGPUDescriptorHandleForHeapStart();
			descriptorSize = device->GetDescriptorHandleIncrementSize(type);
			capacity = cap;
			freeSlots.resize(cap);
			std::iota(freeSlots.begin(), freeSlots.end(), 0);
		}

		void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* cpu, D3D12_GPU_DESCRIPTOR_HANDLE* gpu) {
			uint32_t slot = freeSlots.back();
			freeSlots.pop_back();
			cpu->ptr = cpuStart.ptr + slot * descriptorSize;
			gpu->ptr = gpuStart.ptr + slot * descriptorSize;
		}

		void Free(D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE) {
			uint32_t slot = (uint32_t)((cpu.ptr - cpuStart.ptr) / descriptorSize);
			freeSlots.push_back(slot);
		}
	};

	struct ImguiInitInfo
	{
		ID3D12Device* device = nullptr;
		ID3D12CommandQueue* commandQueue = nullptr;
		ID3D12GraphicsCommandList* commandList = nullptr;
		ID3D12DescriptorHeap* imguiSRVDescriptorHeap = nullptr;
		DescriptorHeapAllocator imguiSrvAllocator;
	};

	class GraphicsManager
	{
	private:
		ComPtr<ID3D12Device> device = nullptr;
		ComPtr<IDXGIFactory6> factory = nullptr;

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
		ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
		ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
		ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
		//-----------------------------------

		//-------------SWAP_CHAIN-------------
		ComPtr<IDXGISwapChain4> swapChain = nullptr;
		std::vector<ComPtr<ID3D12Resource>> backBuffers;
		//-----------------------------------

		//-------------DESCRIPTOR_HEAP-------------
		ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = nullptr;
		//-----------------------------------------

		//-------------DEPTH_STENCIL_VIEW----------
		ComPtr<ID3D12Resource> depthStencilBuffer = nullptr;
		ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = nullptr;
		//-----------------------------------------

		//----------------PIPELINE_STATE-----------
		ComPtr<ID3D12PipelineState> pipelineState = nullptr;
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};
		//-----------------------------------------

		//----------------VIEWPORT-----------------
		D3D12_VIEWPORT viewport = {};
		//-----------------------------------------

		//----------------SCISSOR_RECT-------------
		D3D12_RECT scissorRect = {};
		//-----------------------------------------

		//----------------DEBUG_LAYER--------------
		ComPtr<ID3D12Debug> debugLayer = nullptr;
		//-----------------------------------------

		//------------------FENCE------------------
		ID3D12Fence* fence = nullptr;
		UINT64 _fenceVal = 0;
		//-----------------------------------------

		//------------------IMGUI------------------
		ComPtr<ID3D12DescriptorHeap> imguiSRVDescriptorHeap = nullptr;
		DescriptorHeapAllocator imguiSrvAllocator;
		ImguiInitInfo initInfo;
		//-----------------------------------------

		UINT bbIdx = 0;

		static constexpr int defaultWindowWidth = 1280;
		static constexpr int defaultWindowHeight = 720;

		void DebugOutputFormatString(const char* format, ...);
		void EnableDebugLayer();

		//void CreateImgGuiResources();

		void CreateDepthStencilView();
		void CreateViewPort();
		void CreateScissorRect();
		void CreateSamplerDescriptorHeap();
		void CreateRenderTargetViews();
		void CreatePipelineState();

	public:
		ImguiInitInfo* GetImguiInitInfo();

		void FrameStart();
		void ImguiDescriptorSet();
		void Render();
		void FrameEnd();

		GraphicsManager(HWND hwnd);
		~GraphicsManager() = default;
	};
}
