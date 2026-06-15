module;
#include <Graphics/d3dx12.h>

#include <dxgi1_6.h>
#include <vector>
#include <numeric>
#include <DirectXMath.h>

#include <wrl/client.h>

#include <EngineApi/EngineApi.h>
export module GraphicsManager;

import EngineContext;
import GraphicsContext;
import Vertex;
import Registry;
import TextureManager;
import DescriptorHeap;
import CameraManager;

using Microsoft::WRL::ComPtr;

export namespace Umi
{
	struct SceneMatrix
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
		//DirectX::XMFLOAT3 eye;
	};

	struct ENGINE_API DescriptorHeapAllocator {
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

	class ENGINE_API GraphicsManager
	{
	private:

#pragma region General
		ComPtr<ID3D12Device> device = nullptr;
		ComPtr<IDXGIFactory6> factory = nullptr;

		//--------------FEATURE_LEVEL--------------
		const D3D_FEATURE_LEVEL fls[4] =
		{
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};
		D3D_FEATURE_LEVEL featureLevel;
		//-----------------------------------------

		//--------------COMMAND_LIST---------------
		ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
		ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
		ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
		//-----------------------------------------

		//---------------SWAP_CHAIN----------------
		ComPtr<IDXGISwapChain4> swapChain = nullptr;
		std::vector<ComPtr<ID3D12Resource>> backBuffers;
		//-----------------------------------------

		//-------------DESCRIPTOR_HEAP-------------
		ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = nullptr;
		//-----------------------------------------

		//-------------DEPTH_STENCIL_VIEW----------
		ComPtr<ID3D12Resource> depthStencilBuffer = nullptr;
		ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = nullptr;
		//-----------------------------------------

		//----------------VIEWPORT-----------------
		D3D12_VIEWPORT viewport = {};
		//-----------------------------------------

		//----------------SCISSOR_RECT-------------
		D3D12_RECT scissorRect = {};
		//-----------------------------------------

		//-----------PERMANENT_RESOURCE------------
		ComPtr<ID3D12Resource> peraResource;
		ComPtr<ID3D12DescriptorHeap> peraRTVHeap;
		ComPtr<ID3D12DescriptorHeap> peraSRVHeap;

		ComPtr<ID3D12Resource> peraVB;
		D3D12_VERTEX_BUFFER_VIEW peraVBV = {};
		Vertex2D pv[4] = {
			{{-1.0f,-1.0f, 0.0f}, {0.0f,1.0f} },//左下
			{{-1.0f, 1.0f, 0.0f} ,{0.0f,0.0f}},//左上
			{{ 1.0f,-1.0f, 0.0f} ,{1.0f,1.0f}},//右下
			{{ 1.0f, 1.0f, 0.0f} ,{1.0f,0.0f}},//右上
		};

		ComPtr<ID3D12PipelineState> pipelinestatePera;
		ComPtr<ID3D12RootSignature> rootsignaturePera;
		//-----------------------------------------

		//------------------FENCE------------------
		ID3D12Fence* fence = nullptr;
		UINT64 _fenceVal = 0;
		//-----------------------------------------

		//------------------IMGUI------------------
		ComPtr<ID3D12DescriptorHeap> imguiSRVDescriptorHeap;
		DescriptorHeapAllocator imguiSrvAllocator;
		ImguiInitInfo initInfo;
		//-----------------------------------------
#pragma endregion General

#pragma region 2D
		//--------------2D_VERTICES----------------
		Vertex2D vertices2D[4] = {
			{{-0.5f,-0.5f, 0.0f}, {0.0f,1.0f} },//左下
			{{-0.5f, 0.5f, 0.0f} ,{0.0f,0.0f}},//左上
			{{ 0.5f,-0.5f, 0.0f} ,{1.0f,1.0f}},//右下
			{{ 0.5f, 0.5f, 0.0f} ,{1.0f,0.0f}},//右上
		};
		//-----------------------------------------

		//--------------2D_INDICES-----------------
		unsigned short indices[6] = { 0,1,2, 2,1,3 };
		//-----------------------------------------

		//------------2D_VERTEX_BUFFER--------------
		ComPtr<ID3D12Resource> vertexBuffer2D = nullptr;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView2D = {};
		//-----------------------------------------

		//------------2D_INDEX_BUFFER---------------
		ComPtr<ID3D12Resource> indexBuffer2D = nullptr;
		D3D12_INDEX_BUFFER_VIEW indexBufferView2D = {};
		//-----------------------------------------

		//---------------2D_SHADERS----------------
		ComPtr<ID3DBlob> vertexShaderBlob2D = nullptr;
		ComPtr<ID3DBlob> pixelShaderBlob2D = nullptr;
		//-----------------------------------------

		//-------------ROOT_SIGNATURE--------------
		ComPtr<ID3D12RootSignature> rootsignature2D = nullptr;
		//-----------------------------------------

		//-------------PIPELINE_STATE--------------
		ComPtr<ID3D12PipelineState> pipelinestate2D = nullptr;
		//-----------------------------------------

		//-----------2D_DESCRIPTOR_HEAP------------
		DescriptorHeap descriptorHeap2D;
		//-----------------------------------------

		//--------2D_MATRIX_CONSTANT_BUFFER--------
		ComPtr<ID3D12Resource> matrixConstantBuffer2D;
		SceneMatrix* mapMatrix2D = nullptr;
		//-----------------------------------------
#pragma endregion 2D

#pragma region 3D
		//---------------3D_SHADERS----------------
		ComPtr<ID3DBlob> vertexShaderBlob3D = nullptr;
		ComPtr<ID3DBlob> pixelShaderBlob3D = nullptr;
		//-----------------------------------------

		//-------------ROOT_SIGNATURE--------------
		ComPtr<ID3D12RootSignature> rootsignature3D = nullptr;
		//-----------------------------------------

		//-------------PIPELINE_STATE--------------
		ComPtr<ID3D12PipelineState> pipelinestate3D = nullptr;
		//-----------------------------------------

		//-----------3D_DESCRIPTOR_HEAP------------
		DescriptorHeap descriptorHeap3D;
		//-----------------------------------------

		//--------3D_MATRIX_CONSTANT_BUFFER--------
		ComPtr<ID3D12Resource> matrixConstantBuffer3D;
		SceneMatrix* mapMatrix3D = nullptr;
		//-----------------------------------------
#pragma endregion 3D

#pragma region ImGui
		//-------------EDITOR_VIEWPORT-------------
		D3D12_CPU_DESCRIPTOR_HANDLE viewportSrvCpu{};
		D3D12_GPU_DESCRIPTOR_HANDLE viewportSrvGpu{};
		uint32_t viewportWidth = defaultWindowWidth;
		uint32_t viewportHeight = defaultWindowHeight;
		uint32_t pendingViewportWidth = 0;
		uint32_t pendingViewportHeight = 0;
		bool     viewportResizePending = false;
		static constexpr DXGI_FORMAT viewportColorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		void CreateViewportTargets(uint32_t width, uint32_t height);
		void CreateViewportSRV();
		void InitViewport();
		void ApplyPendingViewportResize();
		//-----------------------------------------
#pragma endregion ImGui

		UINT bbIdx = 0;

		CameraManager cameraManager;

		EngineContext& engineContext;
		GraphicsContext graphicsContext{ descriptorHeap2D, descriptorHeap3D };

		static constexpr int defaultWindowWidth = 1200;
		static constexpr int defaultWindowHeight = 720;

		void DebugOutputFormatString(const char* format, ...);
		void EnableDebugLayer();

		//void CreateImgGuiResources();

		void CreateDepthStencilView();
		void CreateViewPort();
		void CreateScissorRect();
		void CreateSamplerDescriptorHeap();

		void CreatePeraResource();
		void CreatePeraRTVHeap();
		void CreatePeraSRVHeap();
		void CreatePeraPipelineState();

		//----------------2D----------------
		void Create2DVertexBuffer();
		void Create2DIndexBuffer();
		void Load2DShaders();
		void Create2dDescriptorHeap();
		void Create2DMatrixContantBuffer();
		void Create2DPipelineState();
		//----------------------------------

		//----------------3D----------------
		void Load3DShaders();
		void Create3DDescriptorHeap();
		void Create3DMatrixContantBuffer();
		void Create3DPipelineState();
		//----------------------------------


		void Render2D();
		void Render3D();

		void FlushGPU();

	public:

		D3D12_GPU_DESCRIPTOR_HANDLE GetViewportTextureHandle() const noexcept { return viewportSrvGpu; }
		void RequestViewportResize(uint32_t width, uint32_t height);

		ImguiInitInfo* GetImguiInitInfo();

		void Resize(int width, int height);

		void FrameStart();
		void Render();
		void StartImguiFrame();
		void FrameEnd();

		GraphicsContext& GetGraphicsContext() noexcept { return graphicsContext; }

		GraphicsManager(HWND hwnd, EngineContext& engineContext);
		~GraphicsManager();
	};
}
