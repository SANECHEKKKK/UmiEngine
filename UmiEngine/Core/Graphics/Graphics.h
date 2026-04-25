#pragma once

#define NOMINMAX
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <d2d1.h>
#include <dwrite.h>
using namespace DirectX;

#define SAFE_RELEASE(o)  if (o) { (o)->Release(); o = nullptr; }


namespace Umi
{
	class Window;
	struct EngineContext;

	class Graphics
	{
	private:
		friend class Window;

		EngineContext& engineContext;

		ID3D11Device* g_Device = nullptr;
		ID3D11DeviceContext* g_DeviceContext = nullptr;
		IDXGISwapChain* g_SwapChain = nullptr;
		ID3D11Texture2D* g_DepthStencilTexture = nullptr;
		ID3D11RenderTargetView* g_RenderTargetView = nullptr;
		ID3D11DepthStencilView* g_DepthStencilView = nullptr;
		ID3D11RasterizerState* g_RasterizerStateDefault = nullptr;
		ID3D11RasterizerState* g_RasterizerStateScissors = nullptr;
		ID3D11BlendState* g_BlendState = nullptr;
		ID3D11DepthStencilState* g_DepthStencilStateDepthDisable = nullptr;
		ID3D11DepthStencilState* g_DepthStencilStateDepthEnable = nullptr;

		ID2D1Factory* d2dFactory = nullptr;
		ID2D1RenderTarget* d2dRenderTarget = nullptr;
		IDWriteFactory* dwriteFactory = nullptr;

		bool vsyncEnabled = true;

		HWND hWnd = nullptr;


	public:
		ID3D11Device* DirectXGetDevice() const noexcept;
		ID3D11DeviceContext* DirectXGetDeviceContext() const noexcept;
		IDXGISwapChain* DirectXGetSwapChain() const noexcept;

		ID2D1RenderTarget* GetD2DRenderTarget() const noexcept;
		IDWriteFactory* GetDWriteFactory() const noexcept;

		void SetVSyncEnabled(bool enabled) noexcept { vsyncEnabled = enabled; }

		void Clear();
		void Present();

		void Reset();
		void SetFullScreen(bool mode);

		Graphics(HWND hWnd, EngineContext& engineContext);
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;
		~Graphics();
	};
}