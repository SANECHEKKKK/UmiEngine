module;
#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <stdexcept>

#include <Graphics/d3dx12.h>

#include <wrl/client.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

module GraphicsManager;

using Microsoft::WRL::ComPtr;
using namespace Umi;

import Registry;
import Texture;
import Transform;
import Error;

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

void GraphicsManager::CreateRenderTargetViews()
{
	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.LogicOpEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	pipelineStateDesc.BlendState.RenderTarget[0] = renderTargetBlendDesc;
}

void GraphicsManager::Create2DVertexBuffer()
{
	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices2D));


	auto result = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer2D)
	);
	if (result != S_OK)
	{
		Error::FatalError("Failed to create vertex buffer.");
	}

	Vertex2D* vertMap = nullptr;
	result = vertexBuffer2D->Map(0, nullptr, (void**)&vertMap);
	if (result != S_OK)
	{
		Error::FatalError("Failed to map vertex buffer.");
	}

	std::copy(std::begin(vertices2D), std::end(vertices2D), vertMap);

	vertexBuffer2D->Unmap(0, nullptr);

	vertexBufferView2D.BufferLocation = vertexBuffer2D->GetGPUVirtualAddress();
	vertexBufferView2D.SizeInBytes = sizeof(Vertex2D) * std::size(vertices2D);
	vertexBufferView2D.StrideInBytes = sizeof(Vertex2D);
}

void GraphicsManager::Create2DIndexBuffer()
{
	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(indices));

	auto result = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuffer2D)
	);
	if (result != S_OK)
	{
		Error::FatalError("Failed to create index buffer.");
	}

	unsigned short* mappedIdx = nullptr;

	indexBuffer2D->Map(0, nullptr, (void**)&mappedIdx);
	std::copy(std::begin(indices), std::end(indices), mappedIdx);
	indexBuffer2D->Unmap(0, nullptr);

	indexBufferView2D.BufferLocation = indexBuffer2D->GetGPUVirtualAddress();
	indexBufferView2D.Format = DXGI_FORMAT_R16_UINT;
	indexBufferView2D.SizeInBytes = sizeof(indices);
}

void GraphicsManager::Load2DShaders()
{
	ID3DBlob* errorBlob = nullptr;

	auto result = D3DCompileFromFile(
		L"2DBasicVertexShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicVS",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexShaderBlob2D,
		&errorBlob
	);

	if (FAILED(result))
	{
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			Error::FatalError("Shader file not found.");
		}
		else
		{
			std::string errorMessage;
			errorMessage.resize(errorBlob->GetBufferSize());
			std::copy_n(static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize(), errorMessage.begin());
			errorMessage += "\n";
			Error::FatalError(errorMessage);
		}
	}

	result = D3DCompileFromFile(
		L"2DBasicPixelShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicPS",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelShaderBlob2D,
		&errorBlob
	);

	if (result != S_OK)
	{
		std::string errorMessage;
		errorMessage.resize(errorBlob->GetBufferSize());
		std::copy_n(static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize(), errorMessage.begin());
		Error::FatalError("Failed to compile pixel shader." + errorMessage);
	}
}

void GraphicsManager::Create2DPipelineState()
{
	Create2DVertexBuffer();
	Create2DIndexBuffer();
	Load2DShaders();

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	{
		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
	{
		"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob2D.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob2D.Get());

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//中身は0xffffffff

	gpipeline.BlendState.AlphaToCoverageEnable = false;
	gpipeline.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};

	//ひとまず加算や乗算やαブレンディングは使用しない
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//ひとまず論理演算は使用しない
	renderTargetBlendDesc.LogicOpEnable = false;

	gpipeline.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	gpipeline.RasterizerState.MultisampleEnable = false;//まだアンチェリは使わない
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//中身を塗りつぶす
	gpipeline.RasterizerState.DepthClipEnable = true;//深度方向のクリッピングは有効に

	//残り
	gpipeline.RasterizerState.FrontCounterClockwise = false;
	gpipeline.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	gpipeline.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	gpipeline.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	gpipeline.RasterizerState.AntialiasedLineEnable = false;
	gpipeline.RasterizerState.ForcedSampleCount = 0;
	gpipeline.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;


	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpipeline.DepthStencilState.StencilEnable = false;

	gpipeline.InputLayout.pInputElementDescs = inputLayout;//レイアウト先頭アドレス
	gpipeline.InputLayout.NumElements = _countof(inputLayout);//レイアウト配列数

	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;//ストリップ時のカットなし
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//三角形で構成

	gpipeline.NumRenderTargets = 1;//今は１つのみ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～1に正規化されたRGBA

	gpipeline.SampleDesc.Count = 1;//サンプリングは1ピクセルにつき１
	gpipeline.SampleDesc.Quality = 0;//クオリティは最低

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descTblRange = {};
	descTblRange.NumDescriptors = 1;//テクスチャひとつ
	descTblRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//種別はテクスチャ
	descTblRange.BaseShaderRegister = 0;//0番スロットから
	descTblRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	D3D12_ROOT_PARAMETER rootparam = {};
	rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam.DescriptorTable.pDescriptorRanges = &descTblRange;//デスクリプタレンジのアドレス
	rootparam.DescriptorTable.NumDescriptorRanges = 1;//デスクリプタレンジ数
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダから見える

	rootSignatureDesc.pParameters = &rootparam;//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = 1;//ルートパラメータ数

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//横繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//縦繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//奥行繰り返し
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//補間しない(ニアレストネイバー)
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;//ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//オーバーサンプリングの際リサンプリングしない？
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダからのみ可視

	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	ID3DBlob* errorBlob = nullptr;

	ID3DBlob* rootSigBlob2D = nullptr;
	auto result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob2D, &errorBlob);
	if (result != S_OK)
	{
		std::string errorMessage;
		errorMessage.resize(errorBlob->GetBufferSize());
		std::copy_n(static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize(), errorMessage.begin());
		throw "Failed to Serializer 2D Root Signature." + errorMessage;
	}

	result = device->CreateRootSignature(0, rootSigBlob2D->GetBufferPointer(), rootSigBlob2D->GetBufferSize(), IID_PPV_ARGS(&rootsignature2D));
	if (result != S_OK)
	{
		throw "Failed to create 2D root signature.";
	}
	rootSigBlob2D->Release();

	gpipeline.pRootSignature = rootsignature2D.Get();
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate2D));
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


	//ID3D12DescriptorHeap* heaps[] = { imguiSRVDescriptorHeap.Get() };
	//commandList->SetDescriptorHeaps(1, heaps);
}

void GraphicsManager::Render()
{
	commandList->SetPipelineState(pipelinestate2D.Get());

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView2D);
	commandList->IASetIndexBuffer(&indexBufferView2D);
	
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetGraphicsRootSignature(rootsignature2D.Get());
	for (auto e : engineContext.registry.View<Texture, Transform>())
	{
		auto texture = engineContext.registry.GetComponent<Texture>(e);
		auto transform = engineContext.registry.GetComponent<Transform>(e);
		
		TextureData texData = engineContext.textureManager.GetTextureData(texture.id);
		auto& texDescHeap = texData.texDescHeap;

		commandList->SetDescriptorHeaps(1, texDescHeap.GetAddressOf());
		commandList->SetGraphicsRootDescriptorTable(0, texDescHeap->GetGPUDescriptorHandleForHeapStart());

		commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}

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
	swapChain->Present(1, 0);
}

GraphicsManager::GraphicsManager(HWND hwnd, EngineContext& engineContext)
	: engineContext(engineContext)
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

	swapchainDesc.Width = 1200;
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
	Create2DPipelineState();
	//CreatePipelineState();

	graphicsContext = GraphicsContext(
		device.Get(),
		commandAllocator.Get(),
		commandList.Get(),
		commandQueue.Get());
}