module;
#include <windows.h>
#include <dxgi1_6.h>
#include <vector>

#include <Graphics/d3dx12.h>

#include <wrl/client.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

module GraphicsManager;

using Microsoft::WRL::ComPtr;
using namespace Umi;

void GraphicsManager::DebugOutputFormatString(const char* format, ...) 
{
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif
}

void GraphicsManager::EnableDebugLayer()
{
#ifdef _DEBUG
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

	if (result == S_OK)
	{
		debugLayer->EnableDebugLayer();
		debugLayer->Release();
	}
#endif // _DEBUG
}

//void GraphicsManager::CreateImgGuiResources()
//{
//	constexpr uint32_t SRV_HEAP_SIZE = 64;
//
//	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
//	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	heapDesc.NumDescriptors = SRV_HEAP_SIZE;
//	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//	device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_srvHeap));
//
//	m_srvAllocator.Init(device, m_srvHeap.Get(), SRV_HEAP_SIZE);
//
//	// Init ImGui
//	ImGui_ImplDX12_InitInfo initInfo{};
//	initInfo.Device = device;
//	initInfo.CommandQueue = commandQueue;
//	initInfo.NumFramesInFlight = NUM_FRAMES_IN_FLIGHT;
//	initInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
//	initInfo.DSVFormat = DXGI_FORMAT_UNKNOWN; // or your depth format
//	initInfo.SrvDescriptorHeap = m_srvHeap.Get();
//	initInfo.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* cpu, D3D12_GPU_DESCRIPTOR_HANDLE* gpu) {
//		g_srvAllocator.Alloc(cpu, gpu);
//		};
//	initInfo.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu) {
//		g_srvAllocator.Free(cpu, gpu);
//		};
//
//}

void GraphicsManager::CreateRenderTargetViews()
{
	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.LogicOpEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	pipelineStateDesc.BlendState.RenderTarget[0] = renderTargetBlendDesc;
}

void GraphicsManager::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC depthResDesc = {};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = defaultWindowWidth;
	depthResDesc.Height = defaultWindowHeight;
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES depthHeapProp = {};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_CLEAR_VALUE depthClearValue = {};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	ID3D12Resource* depthStencilBuffer = nullptr;
	auto result = device->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthStencilBuffer));
	if (result != S_OK)
	{
		throw "Failed to create depth buffer.";
	}

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ID3D12DescriptorHeap* dsvDescriptorHeap = nullptr;
	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvDescriptorHeap));
	if (result != S_OK)
	{
		throw "Failed to create descriptor heap.";
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(
		depthStencilBuffer,
		&dsvDesc,
		dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void GraphicsManager::CreatePipelineState()
{
	//pipelineStateDesc.pRootSignature = nullptr;

	////pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob);
	////pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob);
	////pipelineStateDesc.VS.pShaderBytecode = vertexShaderBlob->GetBufferPointer();
	////pipelineStateDesc.VS.BytecodeLength = vertexShaderBlob->GetBufferSize();
	////pipelineStateDesc.PS.pShaderBytecode = pixelShaderBlob->GetBufferPointer();
	////pipelineStateDesc.PS.BytecodeLength = pixelShaderBlob->GetBufferSize();

	//pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;


	//pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	////pipelineStateDesc.RasterizerState.MultisampleEnable = false;
	////pipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	////pipelineStateDesc.RasterizerState.DepthClipEnable = true;
	//pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	//pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	////pipelineStateDesc.BlendState.AlphaToCoverageEnable = false;
	////pipelineStateDesc.BlendState.IndependentBlendEnable = false;

	////pipelineStateDesc.InputLayout.pInputElementDescs = inputLayout;
	////pipelineStateDesc.InputLayout.NumElements = _countof(inputLayout);

	//pipelineStateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	//pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//pipelineStateDesc.NumRenderTargets = 1;
	//pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	//pipelineStateDesc.SampleDesc.Count = 1;
	//pipelineStateDesc.SampleDesc.Quality = 0;

	//pipelineStateDesc.DepthStencilState.DepthEnable = true;
	//pipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//pipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	//pipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//auto result = device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState));
	//if (result != S_OK)
	//{
	//	throw "Failed to create pipeline state.";
	//}
}

void GraphicsManager::CreateViewPort()
{
	viewport.Width = static_cast<float>(defaultWindowWidth);
	viewport.Height = static_cast<float>(defaultWindowHeight);
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;
}

void GraphicsManager::CreateScissorRect()
{
	scissorRect.top = 0;
	scissorRect.left = 0;
	scissorRect.right = scissorRect.left + static_cast<LONG>(defaultWindowWidth);
	scissorRect.bottom = scissorRect.top + static_cast<LONG>(defaultWindowHeight);
}

void GraphicsManager::CreateSamplerDescriptorHeap()
{
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Init(0);

	//CD3DX12_STATIC_SAMPLER_DESC samplerDesc[2] = {};
	//samplerDesc[0].Init(0);
	//samplerDesc[1].Init(
	//	1,
	//	D3D12_FILTER_ANISOTROPIC,
	//	D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	//	D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
}

ImguiInitInfo* GraphicsManager::GetImguiInitInfo()
{
	constexpr uint32_t SRV_HEAP_SIZE = 64;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NumDescriptors = SRV_HEAP_SIZE;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&imguiSRVDescriptorHeap));

	imguiSrvAllocator.Init(device.Get(), imguiSRVDescriptorHeap.Get(), SRV_HEAP_SIZE);

	// Init ImGui
	initInfo.device = device.Get();
	initInfo.commandQueue = commandQueue.Get();
	initInfo.commandList = commandList.Get();
	initInfo.imguiSRVDescriptorHeap = imguiSRVDescriptorHeap.Get();
	initInfo.imguiSrvAllocator = imguiSrvAllocator;

	return &initInfo;
}

void GraphicsManager::FrameStart()
{

}

void GraphicsManager::Render()
{
	bbIdx = swapChain->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.Transition.pResource = backBuffers[bbIdx].Get();
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList->ResourceBarrier(1, &BarrierDesc);

	//レンダーターゲットを指定
	auto rtvH = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	commandList->OMSetRenderTargets(1, &rtvH, false, nullptr);

	//画面クリア
	float clearColor[] = { 0.0f,0.0f,0.0f,1.0f };//黒
	commandList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);


	ID3D12DescriptorHeap* heaps[] = { imguiSRVDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(1, heaps);


}

void GraphicsManager::FrameEnd()
{
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.Transition.pResource = backBuffers[bbIdx].Get();
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	commandList->ResourceBarrier(1, &BarrierDesc);

	//命令のクローズ
	commandList->Close();


	//コマンドリストの実行
	ID3D12CommandList* cmdlists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(1, cmdlists);
	////待ち
	commandQueue->Signal(fence, ++_fenceVal);

	if (fence->GetCompletedValue() != _fenceVal) {
		auto event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(_fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
	commandAllocator->Reset();//キューをクリア
	commandList->Reset(commandAllocator.Get(), nullptr);//再びコマンドリストをためる準備

	//フリップ
	swapChain->Present(0, 0);
}

GraphicsManager::GraphicsManager(HWND hwnd)
{
	//-----------DEVICE-----------
	for (auto fv : fls)
	{
		if (D3D12CreateDevice(
			nullptr,
			fv,
			IID_PPV_ARGS(&device)
		) == S_OK)
		{
			featureLevel = fv;
			break;
		}
	}
	//----------------------------


	//-----------FACTORY-----------
	CreateDXGIFactory1(IID_PPV_ARGS(&factory));
	//-----------------------------


	//-----------COMMAND_ALLOCATOR-----------
	if (device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator)) != S_OK)
	{
		throw "Failed to create command allocator.";
	}
	//---------------------------------------


	//-----------COMMAND_LIST-----------
	if (device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList)) != S_OK)
	{
		throw "Failed to create command list.";
	}
	//----------------------------------


	//-----------COMMAND_QUEUE-----------
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	if (device->CreateCommandQueue(
		&commandQueueDesc,
		IID_PPV_ARGS(&commandQueue)) != S_OK)
	{
		throw "Failed to create command queue.";
	}
	//-----------------------------------


	//-------------SWAPCHAIN-------------
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};

	swapchainDesc.Width = 1280;
	swapchainDesc.Height = 720;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;

	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (factory->CreateSwapChainForHwnd(
		commandQueue.Get(),
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)swapChain.GetAddressOf()) != S_OK)
	{
		throw "Failed to create swap chain.";
	}
	//-----------------------------------


	//-------------RENDER_TARGET_VIEW_DESCRIPTOR_HEAP-------------
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NodeMask = 0;
	rtvHeapDesc.NumDescriptors = 2;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	if (device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap)) != S_OK)
	{
		throw "Failed to create descriptor heap.";
	}
	//-----------------------------------------


	//-------------BACK_BUFFERS-------------
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChain->GetDesc(&swapChainDesc);

	backBuffers.resize(swapChainDesc.BufferCount);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < swapChainDesc.BufferCount; i++)
	{
		if (swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i])) != S_OK)
		{
			throw "Failed to get back buffer.";
		}
		device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, rtvHandle);
		rtvHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
	//--------------------------------------


	if (device->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)) != S_OK)
	{
		throw "Failed to create fence.";
	}

	CreateDepthStencilView();
	CreateViewPort();
	CreateScissorRect();
	CreateSamplerDescriptorHeap();
	CreateRenderTargetViews();
	CreatePipelineState();
}