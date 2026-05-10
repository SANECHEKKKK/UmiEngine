#pragma once
#include <cstdint>
#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

namespace Umi
{
	class Registry;
	struct EditorContext;

	class ViewPortWindow
	{
	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> colorTexture;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTexture;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;

		uint32_t width = 1;
		uint32_t height = 1;

	public:
		void Resize(ID3D11Device* device, uint32_t newWidth, uint32_t newHeight);
		void BeginRender(ID3D11DeviceContext* context);
		void DrawImGui();

		ID3D11RenderTargetView* GetRTV() const { return rtv.Get(); }
		ID3D11DepthStencilView* GetDSV() const { return dsv.Get(); }
	};
}