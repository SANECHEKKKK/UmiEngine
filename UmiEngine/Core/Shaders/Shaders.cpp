#include <Shaders/Shaders.h>
#include <fstream>
#include <filesystem>

#include <DirectXTK/WICTextureLoader.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

using namespace Umi;

auto exePath = std::filesystem::current_path();
auto shaderPath = exePath
/ ".." / "UmiEngine" / "Core" / "Shaders";


//---------------------------------Shaders2D---------------------------------//
Shaders2D::Shaders2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	HRESULT hr; // 戻り値格納用

	// デバイスとデバイスコンテキストのチェック
	if (!pDevice || !pContext) {
		MessageBox(nullptr, "Device context", "ERROR", MB_OK);
		return;
	}

	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;


	// 事前コンパイル済み頂点シェーダーの読み込み
	std::ifstream ifs_vs(shaderPath / "shaderVertex2D.cso", std::ios::binary);

	if (!ifs_vs) {
		MessageBox(nullptr, "Cant read vertex data\n\nshaderVertex2D.cso", "ERROR", MB_OK);
		return;
	}

	// ファイルサイズを取得
	ifs_vs.seekg(0, std::ios::end); // ファイルポインタを末尾に移動
	std::streamsize filesize = ifs_vs.tellg(); // ファイルポインタの位置を取得（つまりファイルサイズ）
	ifs_vs.seekg(0, std::ios::beg); // ファイルポインタを先頭に戻す

	// バイナリデータを格納するためのバッファを確保
	unsigned char* vsbinary_pointer = new unsigned char[filesize];

	ifs_vs.read((char*)vsbinary_pointer, filesize); // バイナリデータを読み込む
	ifs_vs.close(); // ファイルを閉じる

	// 頂点シェーダーの作成
	hr = g_pDevice->CreateVertexShader(vsbinary_pointer, filesize, nullptr, &g_pVertexShader);

	if (FAILED(hr)) {
		MessageBox(nullptr, "Cant create vertex data", "ERROR", MB_OK);
		delete[] vsbinary_pointer; // メモリリークしないようにバイナリデータのバッファを解放
		return;
	}


	// 頂点レイアウトの定義
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT num_elements = ARRAYSIZE(layout); // 配列の要素数を取得

	// 頂点レイアウトの作成
	hr = g_pDevice->CreateInputLayout(layout, num_elements, vsbinary_pointer, filesize, &g_pInputLayout);

	delete[] vsbinary_pointer; // バイナリデータのバッファを解放

	if (FAILED(hr)) {
		MessageBox(nullptr, "Cant create vertex layout", "ERROR", MB_OK);
		return;
	}


	// 頂点シェーダー用定数バッファの作成
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(DirectX::XMMATRIX); // バッファのサイズ
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // バインドフラグ

	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pVSConstantBuffer);


	// 事前コンパイル済みピクセルシェーダーの読み込み
	std::ifstream ifs_ps(shaderPath / "shaderPixel2D.cso", std::ios::binary);
	if (!ifs_ps) {
		MessageBox(nullptr, "Cant read pixel shader\n\nshaderPixel2D.cso", "ERROR", MB_OK);
		return;
	}

	ifs_ps.seekg(0, std::ios::end);
	filesize = ifs_ps.tellg();
	ifs_ps.seekg(0, std::ios::beg);

	unsigned char* psbinary_pointer = new unsigned char[filesize];
	ifs_ps.read((char*)psbinary_pointer, filesize);
	ifs_ps.close();

	// ピクセルシェーダーの作成
	hr = g_pDevice->CreatePixelShader(psbinary_pointer, filesize, nullptr, &g_pPixelShader);

	delete[] psbinary_pointer; // バイナリデータのバッファを解放

	if (FAILED(hr)) {

	}


	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // <<< Important
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 0;

	//ID3D11SamplerState* samplerState;
	g_pDevice->CreateSamplerState(&samplerDesc, &g_SamplerState);

	g_pContext->PSSetSamplers(0, 1, &g_SamplerState);
}

Shaders2D::~Shaders2D()
{
	//SAFE_RELEASE(g_pPixelShader);
	//SAFE_RELEASE(g_pVSConstantBuffer);
	//SAFE_RELEASE(g_pInputLayout);
	//SAFE_RELEASE(g_pVertexShader);
}

void Shaders2D::SetMatrix(const DirectX::XMMATRIX& matrix)
{
	DirectX::XMFLOAT4X4 transpose;
	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));
	g_pContext->UpdateSubresource(g_pVSConstantBuffer, 0, nullptr, &transpose, 0, 0);
}

void Shaders2D::Begin()
{
	g_pContext->VSSetShader(g_pVertexShader, nullptr, 0);
	g_pContext->PSSetShader(g_pPixelShader, nullptr, 0);
	g_pContext->IASetInputLayout(g_pInputLayout);
	g_pContext->VSSetConstantBuffers(0, 1, &g_pVSConstantBuffer);

	g_pContext->PSSetSamplers(0, 1, &g_SamplerState);
}

ID3D11Device* Shaders2D::DirectXGetDevice() const noexcept
{
	if (g_pDevice != nullptr)
	{
		return g_pDevice;
	}
}

ID3D11DeviceContext* Shaders2D::DirectXGetDeviceContext() const noexcept
{
	if (g_pContext != nullptr)
	{
		return g_pContext;
	}
}
//---------------------------------------------------------------------------//


//---------------------------------Shaders3D---------------------------------//
Shaders3D::Shaders3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	HRESULT hr; // 戻り値格納用

	// デバイスとデバイスコンテキストのチェック
	if (!pDevice || !pContext) {
		MessageBox(nullptr, "Device context", "ERROR", MB_OK);
		return;
	}

	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;


	// 事前コンパイル済み頂点シェーダーの読み込み
	std::ifstream ifs_vs(shaderPath / "shaderVertex3D.cso", std::ios::binary);

	if (!ifs_vs) {
		MessageBox(nullptr, "Cant read vertex data\n\nshaderVertex3D.cso", "ERROR", MB_OK);
		return;
	}

	// ファイルサイズを取得
	ifs_vs.seekg(0, std::ios::end); // ファイルポインタを末尾に移動
	std::streamsize filesize = ifs_vs.tellg(); // ファイルポインタの位置を取得（つまりファイルサイズ）
	ifs_vs.seekg(0, std::ios::beg); // ファイルポインタを先頭に戻す

	// バイナリデータを格納するためのバッファを確保
	unsigned char* vsbinary_pointer = new unsigned char[filesize];

	ifs_vs.read((char*)vsbinary_pointer, filesize); // バイナリデータを読み込む
	ifs_vs.close(); // ファイルを閉じる

	// 頂点シェーダーの作成
	hr = g_pDevice->CreateVertexShader(vsbinary_pointer, filesize, nullptr, &g_pVertexShader);

	if (FAILED(hr)) {
		MessageBox(nullptr, "Cant create vertex data", "ERROR", MB_OK);
		delete[] vsbinary_pointer; // メモリリークしないようにバイナリデータのバッファを解放
		return;
	}


	// 頂点レイアウトの定義
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES",	0, DXGI_FORMAT_R32G32B32A32_SINT,	0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 48,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT num_elements = ARRAYSIZE(layout); // 配列の要素数を取得

	// 頂点レイアウトの作成
	hr = g_pDevice->CreateInputLayout(layout, num_elements, vsbinary_pointer, filesize, &g_pInputLayout);

	delete[] vsbinary_pointer; // バイナリデータのバッファを解放

	if (FAILED(hr)) {
		MessageBox(nullptr, "Cant create vertex layout", "ERROR", MB_OK);
		return;
	}


	D3D11_BUFFER_DESC matrixBufferDesc = {};
	matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = 0;

	g_pDevice->CreateBuffer(&matrixBufferDesc, nullptr, &matrixBuffer);

	// 事前	コンパイル済みピクセルシェーダーの読み込み
	std::ifstream ifs_ps(shaderPath / "shaderPixel3D.cso", std::ios::binary);
	if (!ifs_ps) {
		MessageBox(nullptr, "Cant read pixel shader\n\nshaderPixel3D.cso", "ERROR", MB_OK);
		return;
	}

	ifs_ps.seekg(0, std::ios::end);
	filesize = ifs_ps.tellg();
	ifs_ps.seekg(0, std::ios::beg);

	unsigned char* psbinary_pointer = new unsigned char[filesize];
	ifs_ps.read((char*)psbinary_pointer, filesize);
	ifs_ps.close();

	// ピクセルシェーダーの作成
	hr = g_pDevice->CreatePixelShader(psbinary_pointer, filesize, nullptr, &g_pPixelShader);

	delete[] psbinary_pointer; // バイナリデータのバッファを解放

	if (FAILED(hr)) {

	}

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//ID3D11SamplerState* samplerState;
	g_pDevice->CreateSamplerState(&samplerDesc, &g_SamplerState);

	g_pContext->PSSetSamplers(0, 1, &g_SamplerState);
}

Shaders3D::~Shaders3D()
{
	//SAFE_RELEASE(g_pPixelShader);
	//SAFE_RELEASE(g_pVSConstantBuffer);
	//SAFE_RELEASE(g_pInputLayout);
	//SAFE_RELEASE(g_pVertexShader);
}

void Shaders3D::SetMatrix(const DirectX::XMMATRIX& matrix)
{
	DirectX::XMFLOAT4X4 transpose;
	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));
	g_pContext->UpdateSubresource(g_pVSConstantBuffer, 0, nullptr, &transpose, 0, 0);
}

void Shaders3D::Begin()
{
	g_pContext->VSSetShader(g_pVertexShader, nullptr, 0);
	g_pContext->PSSetShader(g_pPixelShader, nullptr, 0);
	g_pContext->IASetInputLayout(g_pInputLayout);
	g_pContext->VSSetConstantBuffers(0, 1, &g_pVSConstantBuffer);

	g_pContext->PSSetSamplers(0, 1, &g_SamplerState);
}

ID3D11Device* Shaders3D::DirectXGetDevice() const noexcept
{
	if (g_pDevice != nullptr)
	{
		return g_pDevice;
	}
	else
		return nullptr;
}

ID3D11DeviceContext* Shaders3D::DirectXGetDeviceContext() const noexcept
{
	if (g_pContext != nullptr)
	{
		return g_pContext;
	}
	else
		return nullptr;
}

ID3D11SamplerState* Shaders3D::DirectXGetSamplerState() const noexcept
{
	if (g_SamplerState != nullptr)
	{
		return g_SamplerState;
	}
	else
		return nullptr;
}
//---------------------------------------------------------------------------//