module;
#include <windows.h>
#include <Graphics/d3dx12.h>
#include <dxgi1_6.h>
#include <vector>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

module GraphicsManager;

using namespace Umi;

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
		commandAllocator,
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
		commandQueue,
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&swapChain) != S_OK)
	{
		throw "Failed to create swap chain.";
	}
	//-----------------------------------


	//-------------DESCRIPTOR_HEAP-------------
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
		device->CreateRenderTargetView(backBuffers[0], nullptr, rtvHandle);
		rtvHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
	//--------------------------------------
}