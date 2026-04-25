#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

namespace Umi
{
	struct MatrixBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	class Shaders2D
	{
	private:
		ID3D11VertexShader* g_pVertexShader = nullptr;
		ID3D11InputLayout* g_pInputLayout = nullptr;
		ID3D11Buffer* g_pVSConstantBuffer = nullptr;
		ID3D11PixelShader* g_pPixelShader = nullptr;
		ID3D11SamplerState* g_SamplerState = nullptr;

		ID3D11Device* g_pDevice = nullptr;
		ID3D11DeviceContext* g_pContext = nullptr;

	public:
		void SetMatrix(const DirectX::XMMATRIX& matrix);
		void Begin();
		ID3D11Device* DirectXGetDevice() const noexcept;
		ID3D11DeviceContext* DirectXGetDeviceContext() const noexcept;

		Shaders2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
		~Shaders2D();
	};

	class Shaders3D
	{
		ID3D11VertexShader* g_pVertexShader = nullptr;
		ID3D11InputLayout* g_pInputLayout = nullptr;
		ID3D11Buffer* g_pVSConstantBuffer = nullptr;
		ID3D11PixelShader* g_pPixelShader = nullptr;
		ID3D11SamplerState* g_SamplerState = nullptr;

		ID3D11Device* g_pDevice = nullptr;
		ID3D11DeviceContext* g_pContext = nullptr;

	public:
		ID3D11Buffer* matrixBuffer = nullptr;
		void SetMatrix(const DirectX::XMMATRIX& matrix);
		void Begin();
		ID3D11Device* DirectXGetDevice() const noexcept;
		ID3D11DeviceContext* DirectXGetDeviceContext() const noexcept;
		ID3D11SamplerState* DirectXGetSamplerState() const noexcept;

		Shaders3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
		~Shaders3D();
	};
}