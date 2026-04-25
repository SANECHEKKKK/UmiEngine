#pragma once
#include <d3d11.h>

namespace Umi
{
	class ViewPort
	{
	private:
		ID3D11Texture2D* g_MapTexture = nullptr;//テクスチャデータ
		ID3D11RenderTargetView* g_MapRTV = nullptr;//書込窓口
		ID3D11ShaderResourceView* g_MapSRV = nullptr;//参照窓口

		ID3D11Texture2D* g_MapDepthTexture = nullptr;
		ID3D11DepthStencilView* g_MapDSV = nullptr;

	public:
		ViewPort();
		~ViewPort();
	};
}
