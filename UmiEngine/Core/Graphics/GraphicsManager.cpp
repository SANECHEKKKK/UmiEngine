module;
#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <Graphics/d3dx12.h>

#include <wrl/client.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

module GraphicsManager;

using Microsoft::WRL::ComPtr;
using namespace Umi;

import Registry;
import Error;
import Settings;

import Texture;
import Model;
import Transform;
import Camera;

std::filesystem::path GetExecutableDir() {
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(nullptr, path, MAX_PATH);
	return std::filesystem::path(path).parent_path();
}

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
	ComPtr<ID3D12Debug> debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

	if (result == S_OK)
	{
		debugLayer->EnableDebugLayer();
	}
#endif // _DEBUG
}

//--------------------2D--------------------
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
	auto result = D3DReadFileToBlob(
		(GetExecutableDir() / "2DBasicVertexShader.cso").wstring().c_str(),
		&vertexShaderBlob2D
	);
	if (FAILED(result))
	{
		Error::FatalError("Failed to read 2D vertex shader file.");
	}

	result = D3DReadFileToBlob(
		(GetExecutableDir() / "2DBasicPixelShader.cso").wstring().c_str(),
		&pixelShaderBlob2D
	);
	if (FAILED(result))
	{
		Error::FatalError("Failed to read 2D pixel shader file.");
	}
}

void GraphicsManager::Create2dDescriptorHeap()
{
	auto result = descriptorHeap2D.Init(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	if (result != S_OK)
	{
		Error::FatalError("Failed to create 2D descriptor heap.");
	}
}

void GraphicsManager::Create2DMatrixContantBuffer()
{

	auto constBuffHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	auto constBuffresDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(SceneMatrix) + 0xff) & ~0xff);

	auto result = device->CreateCommittedResource(
		&constBuffHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&constBuffresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&matrixConstantBuffer2D)
	);


	result = matrixConstantBuffer2D->Map(0, nullptr, (void**)&mapMatrix2D);

	descriptorHeap2D.Add();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = matrixConstantBuffer2D->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = matrixConstantBuffer2D->GetDesc().Width;
	device->CreateConstantBufferView(&cbvDesc, descriptorHeap2D[0]);

}

void GraphicsManager::Create2DPipelineState()
{
	Create2DVertexBuffer();
	Create2DIndexBuffer();
	Load2DShaders();
	Create2dDescriptorHeap();
	Create2DMatrixContantBuffer();

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
	gpipeline.RasterizerState.DepthClipEnable = false;//深度方向のクリッピングは有効に

	//残り
	gpipeline.RasterizerState.FrontCounterClockwise = false;
	gpipeline.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	gpipeline.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	gpipeline.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	gpipeline.RasterizerState.AntialiasedLineEnable = false;
	gpipeline.RasterizerState.ForcedSampleCount = 0;
	gpipeline.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;


	gpipeline.DepthStencilState.DepthEnable = false;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpipeline.DepthStencilState.StencilEnable = false;

	gpipeline.InputLayout.pInputElementDescs = inputLayout;//レイアウト先頭アドレス
	gpipeline.InputLayout.NumElements = _countof(inputLayout);//レイアウト配列数

	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;//ストリップ時のカットなし
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//三角形で構成

	gpipeline.NumRenderTargets = 1;//今は１つのみ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～1に正規化されたRGBA



	//--------------------SAMPLER--------------------
	gpipeline.SampleDesc.Count = 1;//サンプリングは1ピクセルにつき１
	gpipeline.SampleDesc.Quality = 0;//クオリティは最低

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
	//-----------------------------------------------



	//-----------------DESCRIPTOR_TABLE--------------
	D3D12_DESCRIPTOR_RANGE descTblRange[2] = {};
	descTblRange[0].NumDescriptors = 1;//テクスチャひとつ
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//種別はテクスチャ
	descTblRange[0].BaseShaderRegister = 0;//0番スロットから
	descTblRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	descTblRange[1].NumDescriptors = 1;//テクスチャひとつ
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//種別はテクスチャ
	descTblRange[1].BaseShaderRegister = 0;//0番スロットから
	descTblRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	//-----------------------------------------------

	//-----------------ROOT_PARAMETER----------------
	D3D12_ROOT_PARAMETER rootparam[2] = {};
	rootparam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam[0].DescriptorTable.pDescriptorRanges = &descTblRange[0];//デスクリプタレンジのアドレス
	rootparam[0].DescriptorTable.NumDescriptorRanges = 1;//デスクリプタレンジ数
	rootparam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//頂点シェーダから見える

	rootparam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam[1].DescriptorTable.pDescriptorRanges = &descTblRange[1];//デスクリプタレンジのアドレス
	rootparam[1].DescriptorTable.NumDescriptorRanges = 1;//デスクリプタレンジ数
	rootparam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダから見える
	//-----------------------------------------------



	//-----------------ROOT_SIGNATURE----------------
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootparam;//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = 2;//ルートパラメータ数
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
		Error::FatalError("Failed to Serializer 2D Root Signature." + errorMessage);
	}

	result = device->CreateRootSignature(0, rootSigBlob2D->GetBufferPointer(), rootSigBlob2D->GetBufferSize(), IID_PPV_ARGS(&rootsignature2D));
	if (result != S_OK)
	{
		Error::FatalError("Failed to create 2D root signature.");
	}
	rootSigBlob2D->Release();
	//-----------------------------------------------

	gpipeline.pRootSignature = rootsignature2D.Get();
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate2D));
}
//------------------------------------------

//--------------------3D--------------------
void GraphicsManager::Load3DShaders()
{
	auto result = D3DReadFileToBlob(
		(GetExecutableDir() / "3DBasicVertexShader.cso").wstring().c_str(),
		&vertexShaderBlob3D
	);

	if (FAILED(result))
	{
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			Error::FatalError("3D vertex shader .cso not found");
		}
		else
		{
			Error::FatalError("Failed to load 3D vertex shader .cso.");
		}
	}

	// Load precompiled pixel shader
	result = D3DReadFileToBlob(
		(GetExecutableDir() / "3DBasicPixelShader.cso").wstring().c_str(),
		&pixelShaderBlob3D
	);

	if (FAILED(result))
	{
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			Error::FatalError("3D pixel shader .cso not found");
		}
		else
		{
			Error::FatalError("Failed to load 3D pixel shader .cso.");
		}
	}
}

void GraphicsManager::Create3DDescriptorHeap()
{
	auto result = descriptorHeap3D.Init(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	if (result != S_OK)
	{
		Error::FatalError("Failed to create 3D descriptor heap.");
	}
}

void GraphicsManager::Create3DMatrixContantBuffer()
{
	auto constBuffHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	auto constBuffresDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(SceneMatrix) + 0xff) & ~0xff);

	auto result = device->CreateCommittedResource(
		&constBuffHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&constBuffresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&matrixConstantBuffer3D)
	);


	result = matrixConstantBuffer3D->Map(0, nullptr, (void**)&mapMatrix3D);

	descriptorHeap3D.Add();
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = matrixConstantBuffer3D->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = matrixConstantBuffer3D->GetDesc().Width;
	device->CreateConstantBufferView(&cbvDesc, descriptorHeap3D[0]);

}

void GraphicsManager::Create3DPipelineState()
{
	Load3DShaders();
	Create3DDescriptorHeap();
	Create3DMatrixContantBuffer();

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	{
		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
	{
		"NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
	{
		"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob3D.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob3D.Get());

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
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;//カリングしない
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
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;


	gpipeline.InputLayout.pInputElementDescs = inputLayout;//レイアウト先頭アドレス
	gpipeline.InputLayout.NumElements = _countof(inputLayout);//レイアウト配列数

	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;//ストリップ時のカットなし
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//三角形で構成

	gpipeline.NumRenderTargets = 1;//今は１つのみ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～1に正規化されたRGBA



	//--------------------SAMPLER--------------------
	gpipeline.SampleDesc.Count = 1;//サンプリングは1ピクセルにつき１
	gpipeline.SampleDesc.Quality = 0;//クオリティは最低

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
	//-----------------------------------------------



	//-----------------DESCRIPTOR_TABLE--------------
	//D3D12_DESCRIPTOR_RANGE descTblRange = {};
	//descTblRange.NumDescriptors = 1;//テクスチャひとつ
	//descTblRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//種別はテクスチャ
	//descTblRange.BaseShaderRegister = 0;//0番スロットから
	//descTblRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//descTblRange[1].NumDescriptors = 1;//テクスチャひとつ
	//descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//種別はテクスチャ
	//descTblRange[1].BaseShaderRegister = 0;//0番スロットから
	//descTblRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	//-----------------------------------------------

	//-----------------ROOT_PARAMETER----------------
	//D3D12_ROOT_PARAMETER rootparam = {};
	//rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rootparam.DescriptorTable.pDescriptorRanges = &descTblRange;//デスクリプタレンジのアドレス
	//rootparam.DescriptorTable.NumDescriptorRanges = 1;//デスクリプタレンジ数
	//rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//頂点シェーダから見える

	//rootparam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rootparam[1].DescriptorTable.pDescriptorRanges = &descTblRange[1];//デスクリプタレンジのアドレス
	//rootparam[1].DescriptorTable.NumDescriptorRanges = 1;//デスクリプタレンジ数
	//rootparam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダから見える

	//          NEW WAY
	CD3DX12_ROOT_PARAMETER rootparam[3] = {};
	rootparam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
	rootparam[1].InitAsConstants(12, 1, 0, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_DESCRIPTOR_RANGE range;
	range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT_MAX, 0);
	rootparam[2].InitAsDescriptorTable(1, &range, D3D12_SHADER_VISIBILITY_PIXEL);

	//-----------------------------------------------



	//-----------------ROOT_SIGNATURE----------------
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootparam;//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = 3;//ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	ID3DBlob* errorBlob = nullptr;

	ID3DBlob* rootSigBlob3D = nullptr;
	auto result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob3D, &errorBlob);
	if (result != S_OK)
	{
		std::string errorMessage;
		errorMessage.resize(errorBlob->GetBufferSize());
		std::copy_n(static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize(), errorMessage.begin());
		Error::FatalError("Failed to Serializer 3D Root Signature." + errorMessage);
	}

	result = device->CreateRootSignature(0, rootSigBlob3D->GetBufferPointer(), rootSigBlob3D->GetBufferSize(), IID_PPV_ARGS(&rootsignature3D));
	if (result != S_OK)
	{
		Error::FatalError("Failed to create 3D root signature.");
	}
	rootSigBlob3D->Release();
	//-----------------------------------------------

	gpipeline.pRootSignature = rootsignature3D.Get();
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate3D));
	if (result != S_OK)
	{
		Error::FatalError("Failed to create 3D pipeline state.");
	}

}
//------------------------------------------

void GraphicsManager::CreateViewportTargets(uint32_t width, uint32_t height)
{
	viewportWidth = width;
	viewportHeight = height;

	CD3DX12_HEAP_PROPERTIES defaultHeap(D3D12_HEAP_TYPE_DEFAULT);

	// ---- color (peraResource) ----
	D3D12_RESOURCE_DESC color{};
	color.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	color.Width = width;
	color.Height = height;
	color.DepthOrArraySize = 1;
	color.MipLevels = 1;
	color.Format = viewportColorFormat;
	color.SampleDesc.Count = 1;
	color.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	float cc[4] = { 0.10f, 0.10f, 0.12f, 1.0f };
	D3D12_CLEAR_VALUE colorClear = CD3DX12_CLEAR_VALUE(viewportColorFormat, cc);

	if (device->CreateCommittedResource(
		&defaultHeap, D3D12_HEAP_FLAG_NONE, &color,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&colorClear, IID_PPV_ARGS(peraResource.ReleaseAndGetAddressOf())) != S_OK)
		Error::FatalError("Failed to create viewport color target.");
	peraResource->SetName(L"ViewportColor");

	D3D12_RENDER_TARGET_VIEW_DESC rtv{};
	rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtv.Format = viewportColorFormat;
	device->CreateRenderTargetView(
		peraResource.Get(), &rtv,
		peraRTVHeap->GetCPUDescriptorHandleForHeapStart());

	// ---- depth (depthStencilBuffer) ----
	D3D12_RESOURCE_DESC depth{};
	depth.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depth.Width = width;
	depth.Height = height;
	depth.DepthOrArraySize = 1;
	depth.MipLevels = 1;
	depth.Format = DXGI_FORMAT_D32_FLOAT;
	depth.SampleDesc.Count = 1;
	depth.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthClear{};
	depthClear.Format = DXGI_FORMAT_D32_FLOAT;
	depthClear.DepthStencil.Depth = 1.0f;

	if (device->CreateCommittedResource(
		&defaultHeap, D3D12_HEAP_FLAG_NONE, &depth,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClear, IID_PPV_ARGS(depthStencilBuffer.ReleaseAndGetAddressOf())) != S_OK)
		Error::FatalError("Failed to create viewport depth target.");
	depthStencilBuffer->SetName(L"ViewportDepth");

	D3D12_DEPTH_STENCIL_VIEW_DESC dsv{};
	dsv.Format = DXGI_FORMAT_D32_FLOAT;
	dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(
		depthStencilBuffer.Get(), &dsv,
		dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// Scene pass uses these — panel-sized, not window-sized.
	viewport = { 0.f, 0.f, (float)width, (float)height, 0.f, 1.f };
	scissorRect = { 0, 0, (LONG)width, (LONG)height };
}

void GraphicsManager::CreateViewPort()
{
	viewport.Width = static_cast<float>(Settings::getResolution().width);
	viewport.Height = static_cast<float>(Settings::getResolution().height);
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;
}

void GraphicsManager::CreateScissorRect()
{
	scissorRect.top = 0;
	scissorRect.left = 0;
	scissorRect.right = scissorRect.left + static_cast<LONG>(Settings::getResolution().width);
	scissorRect.bottom = scissorRect.top + static_cast<LONG>(Settings::getResolution().height);
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

void GraphicsManager::CreatePeraRTVHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&peraRTVHeap)) != S_OK)
		Error::FatalError("Failed to create viewport RTV heap.");
}

void GraphicsManager::CreateDepthStencilView()
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	if (device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvDescriptorHeap)) != S_OK)
		Error::FatalError("Failed to create DSV heap.");
}

void GraphicsManager::CreateViewportSRV()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srv{};
	srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv.Format = viewportColorFormat;
	srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(peraResource.Get(), &srv, viewportSrvCpu);
}

void GraphicsManager::InitViewport()
{
	// Reserve a slot from the SAME allocator ImGui uses (the one in initInfo),
	// so the font and the viewport texture never share a descriptor.
	initInfo.imguiSrvAllocator.Alloc(&viewportSrvCpu, &viewportSrvGpu);
	CreateViewportSRV();
}

void GraphicsManager::RequestViewportResize(uint32_t width, uint32_t height)
{
	if (width == 0 || height == 0) return;
	if (width == viewportWidth && height == viewportHeight) return;
	pendingViewportWidth = width;
	pendingViewportHeight = height;
	viewportResizePending = true;
}

void GraphicsManager::ApplyPendingViewportResize()
{
	if (!viewportResizePending) return;
	viewportResizePending = false;

	FlushGPU();
	CreateViewportTargets(pendingViewportWidth, pendingViewportHeight);
	CreateViewportSRV();

	cameraManager.SetViewportSize(pendingViewportWidth, pendingViewportHeight);
}

void GraphicsManager::CreatePeraResource()
{
	auto& bbuf = backBuffers[0];
	auto resDesc = bbuf->GetDesc();

	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };//白
	D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor);

	auto result = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(&peraResource));
	if (result != S_OK)
	{
		Error::FatalError("Failed to create pera resource.");
	}

	auto heapDesc = rtvDescriptorHeap->GetDesc();
	heapDesc.NumDescriptors = 1;
	result = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&peraRTVHeap));
	if (result != S_OK)
	{
		Error::FatalError("Failed to create pera RTV heap.");
	}

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	device->CreateRenderTargetView(peraResource.Get(), &rtvDesc, peraRTVHeap->GetCPUDescriptorHandleForHeapStart());

	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	result = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&peraSRVHeap));
	if (result != S_OK)
	{
		Error::FatalError("Failed to create pera SRV heap.");
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	device->CreateShaderResourceView(peraResource.Get(), &srvDesc, peraSRVHeap->GetCPUDescriptorHandleForHeapStart());

	heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(pv));
	result = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&peraVB));

	peraVBV.BufferLocation = peraVB->GetGPUVirtualAddress();
	peraVBV.SizeInBytes = sizeof(pv);
	peraVBV.StrideInBytes = sizeof(Vertex2D);
	Vertex2D* mapPeraVB = nullptr;
	peraVB->Map(0, nullptr, (void**)&mapPeraVB);
	std::copy(std::begin(pv), std::end(pv), mapPeraVB);
	peraVB->Unmap(0, nullptr);
}

void GraphicsManager::CreatePeraPipelineState()
{
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	{
		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
	{
		"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};
	gpsDesc.InputLayout.NumElements = _countof(inputLayout);
	gpsDesc.InputLayout.pInputElementDescs = inputLayout;

	ComPtr<ID3DBlob> vs;
	ComPtr<ID3DBlob> ps;

	auto result = D3DReadFileToBlob(
		(GetExecutableDir() / "peraVertex.cso").wstring().c_str(),
		&vs
	);

	if (FAILED(result))
	{
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			Error::FatalError("Pera vertex shader .cso not found");
		}
		else
		{
			Error::FatalError("Failed to load Pera vertex shader .cso.");
		}
	}

	// Load precompiled pixel shader
	result = D3DReadFileToBlob(
		(GetExecutableDir() / "peraPixel.cso").wstring().c_str(),
		&ps
	);

	if (FAILED(result))
	{
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			Error::FatalError("Pera pixel shader .cso not found");
		}
		else
		{
			Error::FatalError("Failed to load Pera pixel shader .cso.");
		}
	}

	gpsDesc.VS = CD3DX12_SHADER_BYTECODE(vs.Get());
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(ps.Get());

	gpsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.NumRenderTargets = 1;
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.SampleDesc.Quality = 0;
	gpsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;


	D3D12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);


	D3D12_DESCRIPTOR_RANGE range = {};
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range.BaseShaderRegister = 0;
	range.NumDescriptors = 1;

	D3D12_ROOT_PARAMETER rp = {};
	rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rp.DescriptorTable.pDescriptorRanges = &range;
	rp.DescriptorTable.NumDescriptorRanges = 1;

	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = 1;
	rsDesc.pParameters = &rp;
	rsDesc.NumStaticSamplers = 1;
	rsDesc.pStaticSamplers = &samplerDesc;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> rsBlob;
	ComPtr<ID3DBlob> errBlob;

	result = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rsBlob, &errBlob);
	if (FAILED(result))
	{
		std::string errorMessage;
		errorMessage.resize(errBlob->GetBufferSize());
		std::copy_n(static_cast<char*>(errBlob->GetBufferPointer()), errBlob->GetBufferSize(), errorMessage.begin());
		Error::FatalError("Failed to serialize Pera root signature. " + errorMessage);
	}
	result = device->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignaturePera));
	if (FAILED(result))
	{
		Error::FatalError("Failed to create Pera root signature.");
	}

	gpsDesc.pRootSignature = rootsignaturePera.Get();
	result = device->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(&pipelinestatePera));
	if (FAILED(result))
	{
		Error::FatalError("Failed to create Pera pipeline state.");
	}
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

	initInfo.device = device.Get();
	initInfo.commandQueue = commandQueue.Get();
	initInfo.commandList = commandList.Get();
	initInfo.imguiSRVDescriptorHeap = imguiSRVDescriptorHeap.Get();
	initInfo.imguiSrvAllocator = imguiSrvAllocator;

	InitViewport();

	return &initInfo;
}

void GraphicsManager::Resize(int width, int height)
{
	if (width == 0 || height == 0)
		return;

	FlushGPU();

	for (uint32_t i = 0; i < 2; ++i)
		backBuffers[i].Reset();

	DXGI_SWAP_CHAIN_DESC1 desc{};
	swapChain->GetDesc1(&desc);
	auto result = swapChain->ResizeBuffers(
		2, width, height,
		desc.Format, desc.Flags);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
		rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	for (uint32_t i = 0; i < 2; ++i)
	{
		result = swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		if (result != S_OK)
		{
			Error::FatalError("Failed to get swap chain buffer.");
		}
		device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	}

}

void GraphicsManager::FrameStart()
{
	ApplyPendingViewportResize();

	bbIdx = swapChain->GetCurrentBackBufferIndex();

	auto toRT = CD3DX12_RESOURCE_BARRIER::Transition(
		peraResource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &toRT);

	auto rtvH = peraRTVHeap->GetCPUDescriptorHandleForHeapStart();
	auto dsvH = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	commandList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	commandList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	float clearColor[] = { 0.10f, 0.10f, 0.12f, 1.0f };
	commandList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
}

void GraphicsManager::StartImguiFrame()
{
	auto toRT = CD3DX12_RESOURCE_BARRIER::Transition(
		backBuffers[bbIdx].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &toRT);

	auto rtvH = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	commandList->OMSetRenderTargets(1, &rtvH, false, nullptr);

	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	commandList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	ID3D12DescriptorHeap* heaps[] = { imguiSRVDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(1, heaps);
}

void GraphicsManager::Render2D()
{
	Camera* camera = cameraManager.GetMainCamera();

	commandList->SetPipelineState(pipelinestate2D.Get());

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);


	commandList->IASetVertexBuffers(0, 1, &vertexBufferView2D);
	commandList->IASetIndexBuffer(&indexBufferView2D);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetGraphicsRootSignature(rootsignature2D.Get());

	auto* heap = descriptorHeap2D.Get();
	auto cbvHandle = descriptorHeap2D.GetGPU(0);

	commandList->SetDescriptorHeaps(1, &heap);
	commandList->SetGraphicsRootDescriptorTable(0, cbvHandle);


	for (auto e : engineContext.registry.View<Texture, Transform>())
	{
		auto& texture = engineContext.registry.GetComponent<Texture>(e);
		auto& transform = engineContext.registry.GetComponent<Transform>(e);


		if (texture.id == INVALID_TEXTUREID) continue;//テクスチャがまだないなら描画しない


		auto& texData = engineContext.textureManager.GetTextureData(texture.id);
		auto texHandle = descriptorHeap2D.GetGPU(texData.descriptorHeapIndex);

		DirectX::XMMATRIX world =
			DirectX::XMMatrixScaling(texData.width * transform.scale.x, texData.height * transform.scale.y, 1.0f) *
			DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(transform.rot.x)) * DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(transform.rot.y)) * DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(transform.rot.z)) *
			DirectX::XMMatrixTranslation(transform.pos.x, transform.pos.y, transform.pos.z);

		SceneMatrix matrices{ world, camera->viewMatrix2D, camera->projectionMatrix2D };
		memcpy(mapMatrix2D, &matrices, sizeof(SceneMatrix));

		commandList->SetGraphicsRootDescriptorTable(1, texHandle);
		commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}
}

void GraphicsManager::Render3D()
{
	Camera* camera = cameraManager.GetMainCamera();

	commandList->SetPipelineState(pipelinestate3D.Get());

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetGraphicsRootSignature(rootsignature3D.Get());

	auto* heap = descriptorHeap3D.Get();
	auto cbvHandle = descriptorHeap3D.GetGPU(0);
	cbvHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	commandList->SetDescriptorHeaps(1, &heap);
	commandList->SetGraphicsRootDescriptorTable(2, cbvHandle);

	for (auto e : engineContext.registry.View<Model, Transform>())
	{
		auto& model = engineContext.registry.GetComponent<Model>(e);
		auto& transform = engineContext.registry.GetComponent<Transform>(e);


		if (model.id == INVALID_MODELID)
			continue;


		auto& modelData = engineContext.modelManager.GetModelData(model.id);

		DirectX::XMMATRIX world =
			DirectX::XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z) *
			DirectX::XMMatrixRotationX(transform.rot.x) * DirectX::XMMatrixRotationY(transform.rot.y) * DirectX::XMMatrixRotationZ(transform.rot.z) *
			DirectX::XMMatrixTranslation(transform.pos.x, transform.pos.y, transform.pos.z);

		SceneMatrix matrices{ world, camera->viewMatrix3D, camera->projectionMatrix3D };
		memcpy(mapMatrix3D, &matrices, sizeof(SceneMatrix));
		commandList->SetGraphicsRootConstantBufferView(0, matrixConstantBuffer3D.Get()->GetGPUVirtualAddress());

		for (auto& mesh : modelData.meshes)
		{
			commandList->IASetVertexBuffers(0, 1, &mesh.vertexBufferView);
			commandList->IASetIndexBuffer(&mesh.indexBufferView);

			commandList->SetGraphicsRoot32BitConstants(1, 12, &modelData.materials[mesh.materialIndex], 0);

			commandList->DrawIndexedInstanced(mesh.indexCount, 1, 0, 0, 0);
		}
	}
}

void GraphicsManager::Render()
{
	cameraManager.Update();

	Render2D();
	Render3D();

	auto toSRV = CD3DX12_RESOURCE_BARRIER::Transition(
		peraResource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandList->ResourceBarrier(1, &toSRV);
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
	: engineContext(engineContext),
	cameraManager(engineContext)
{

	EnableDebugLayer();


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
		Error::FatalError("Failed to create command allocator.");
	}
	//---------------------------------------


	//-----------COMMAND_LIST-----------
	if (device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList)) != S_OK)
	{
		Error::FatalError("Failed to create command list.");
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
		Error::FatalError("Failed to create command queue.");
	}
	//-----------------------------------


	//-------------SWAPCHAIN-------------
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};

	swapchainDesc.Width = Settings::getResolution().width;
	swapchainDesc.Height = Settings::getResolution().height;
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
		Error::FatalError("Failed to create swap chain.");
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
		Error::FatalError("Failed to create descriptor heap.");
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
			Error::FatalError("Failed to get back buffer.");
		}
		device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, rtvHandle);
		rtvHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
	//--------------------------------------


	if (device->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)) != S_OK)
	{
		Error::FatalError("Failed to create fence.");
	}

	//CreateDepthStencilView();
	//CreateViewPort();
	//CreateScissorRect();
	CreatePeraRTVHeap();
	CreateDepthStencilView();
	CreateViewportTargets(Settings::getResolution().width,
		Settings::getResolution().height);
	cameraManager.SetViewportSize(Settings::getResolution().width,
		Settings::getResolution().height);
	CreateSamplerDescriptorHeap();

	//------------------2D------------------
	Create2DPipelineState();
	//--------------------------------------

	//------------------3D------------------
	Create3DPipelineState();
	//--------------------------------------

	//-----------------PERA-----------------
	//CreatePeraResource();
	//CreatePeraPipelineState();
	//--------------------------------------

	graphicsContext.device = device.Get();
	graphicsContext.commandAllocator = commandAllocator.Get();
	graphicsContext.commandList = commandList.Get();
	graphicsContext.commandQueue = commandQueue.Get();
	graphicsContext.descriptorHeap2D = descriptorHeap2D;

}

void GraphicsManager::FlushGPU()
{
	if (!commandQueue || !fence) return;

	++_fenceVal;
	commandQueue->Signal(fence, _fenceVal);

	if (fence->GetCompletedValue() < _fenceVal)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		if (event)
		{
			fence->SetEventOnCompletion(_fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
	}
}

GraphicsManager::~GraphicsManager()
{
	FlushGPU();
}