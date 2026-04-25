#include <Graphics/Graphics.h>
#include <EngineContext/EngineContext.h>
#include <Settings/Settings.h>

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

using namespace Umi;

Graphics::Graphics(HWND hWnd, EngineContext& engineContext) : hWnd(hWnd), engineContext(engineContext)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = engineContext.settings.getResolution().width;
	sd.BufferDesc.Height = engineContext.settings.getResolution().height;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;

	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&g_SwapChain,
		&g_Device,
		&feature_level,
		&g_DeviceContext);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed to create device and swap chain", "Error", MB_OK);
		return;
	}
	ID3D11Texture2D* pBackBuffer = NULL;
	g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	g_Device->CreateRenderTargetView(pBackBuffer, NULL, &g_RenderTargetView);
	pBackBuffer->Release();


	D3D11_TEXTURE2D_DESC td = {};
	td.Width = sd.BufferDesc.Width;
	td.Height = sd.BufferDesc.Height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc = sd.SampleDesc;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	g_Device->CreateTexture2D(&td, NULL, &g_DepthStencilTexture);



	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = td.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Flags = 0;
	g_Device->CreateDepthStencilView(g_DepthStencilTexture, &dsvd, &g_DepthStencilView);


	g_DeviceContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	FLOAT vpWidth = static_cast<FLOAT>(clientRect.right - clientRect.left);
	FLOAT vpHeight = static_cast<FLOAT>(clientRect.bottom - clientRect.top);

	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = vpWidth;
	vp.Height = vpHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	g_DeviceContext->RSSetViewports(1, &vp);

	D3D11_BLEND_DESC bd = {};
	bd.AlphaToCoverageEnable = FALSE;
	bd.IndependentBlendEnable = FALSE;
	bd.RenderTarget[0].BlendEnable = TRUE;

	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	float blend_factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_Device->CreateBlendState(&bd, &g_BlendState);

	g_DeviceContext->OMSetBlendState(g_BlendState, blend_factor, 0xffffffff);

	vsyncEnabled = engineContext.settings.isVSyncEnabled();

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed to create Direct2D factory", "Error", MB_OK);
		return;
	}

	IDXGISurface* dxgiBackBuffer = nullptr;
	hr = g_SwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void**)&dxgiBackBuffer);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed to get DXGI surface", "Error", MB_OK);
		return;
	}
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		0, 0,
		D2D1_RENDER_TARGET_USAGE_NONE,
		D2D1_FEATURE_LEVEL_DEFAULT
	);

	hr = d2dFactory->CreateDxgiSurfaceRenderTarget(dxgiBackBuffer, &props, &d2dRenderTarget);
	dxgiBackBuffer->Release();

	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed to create D2D render target", "Error", MB_OK);
		return;
	}

	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&dwriteFactory)
	);

	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed to create DirectWrite factory", "Error", MB_OK);
		return;
	}

}

Graphics::~Graphics()
{
	SAFE_RELEASE(g_Device);
	SAFE_RELEASE(g_DeviceContext);
	SAFE_RELEASE(g_SwapChain);
	SAFE_RELEASE(g_DepthStencilTexture);
	SAFE_RELEASE(g_RenderTargetView);
	SAFE_RELEASE(g_DepthStencilView);
	SAFE_RELEASE(g_BlendState);
	SAFE_RELEASE(g_DepthStencilStateDepthDisable);
	SAFE_RELEASE(d2dRenderTarget);
	SAFE_RELEASE(d2dFactory);
	SAFE_RELEASE(dwriteFactory);
}

ID3D11Device* Graphics::DirectXGetDevice() const noexcept
{
	if (g_Device != nullptr)
	{
		return g_Device;
	}
	else
		return nullptr;
}

ID3D11DeviceContext* Graphics::DirectXGetDeviceContext() const noexcept
{
	if (g_DeviceContext != nullptr)
	{
		return g_DeviceContext;
	}
	else
		return nullptr;
}

IDXGISwapChain* Graphics::DirectXGetSwapChain() const noexcept
{
	if (g_SwapChain != nullptr)
		return g_SwapChain;
	else
		return nullptr;
}

ID2D1RenderTarget* Graphics::GetD2DRenderTarget() const noexcept
{
	if (!d2dRenderTarget)
		return nullptr;
	else
		return d2dRenderTarget;
}

IDWriteFactory* Graphics::GetDWriteFactory() const noexcept
{
	if (!dwriteFactory)
		return nullptr;
	else
		return dwriteFactory;
}

void Graphics::Clear()
{
	float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_DeviceContext->ClearRenderTargetView(g_RenderTargetView, clear_color);
	g_DeviceContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Graphics::Present()
{
	g_SwapChain->Present(vsyncEnabled ? 1 : 0, 0);
}

void Graphics::Reset()
{
	SAFE_RELEASE(g_RenderTargetView);
	SAFE_RELEASE(g_DepthStencilView);
	SAFE_RELEASE(g_DepthStencilTexture);
	SAFE_RELEASE(d2dRenderTarget);
	g_DeviceContext->Flush();

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	UINT clientWidth = static_cast<UINT>(clientRect.right - clientRect.left);
	UINT clientHeight = static_cast<UINT>(clientRect.bottom - clientRect.top);

	g_SwapChain->ResizeBuffers(0, clientWidth, clientHeight, DXGI_FORMAT_UNKNOWN, 0);

	ID3D11Texture2D* backBuffer = nullptr;
	HRESULT hr = g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (SUCCEEDED(hr)) {
		g_Device->CreateRenderTargetView(backBuffer, nullptr, &g_RenderTargetView);
		backBuffer->Release();
	}

	IDXGISurface* dxgiBackBuffer = nullptr;
	hr = g_SwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void**)&dxgiBackBuffer);
	if (SUCCEEDED(hr))
	{
		D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			0, 0,
			D2D1_RENDER_TARGET_USAGE_NONE,
			D2D1_FEATURE_LEVEL_DEFAULT
		);

		hr = d2dFactory->CreateDxgiSurfaceRenderTarget(dxgiBackBuffer, &props, &d2dRenderTarget);
		dxgiBackBuffer->Release();

		if (FAILED(hr))
		{
			MessageBoxA(NULL, "Failed to recreate D2D render target", "Error", MB_OK);
		}
	}

	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = clientWidth;
	depthDesc.Height = clientHeight;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hr = g_Device->CreateTexture2D(&depthDesc, nullptr, &g_DepthStencilTexture);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = depthDesc.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Flags = 0;
	g_Device->CreateDepthStencilView(g_DepthStencilTexture, &dsvd, &g_DepthStencilView);


	g_DeviceContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);

	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<FLOAT>(clientWidth);
	vp.Height = static_cast<FLOAT>(clientHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	g_DeviceContext->RSSetViewports(1, &vp);
}

void Graphics::SetFullScreen(bool mode)
{
	if (g_SwapChain)
	{
		if (mode)
		{
			BOOL fullscreen = TRUE;
			g_SwapChain->SetFullscreenState(fullscreen, nullptr);
		}
		else
		{
			BOOL fullscreen = FALSE;
			g_SwapChain->SetFullscreenState(fullscreen, nullptr);
		}
	}
}