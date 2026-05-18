module;
#include <Graphics/d3dx12.h>

#include <DirectXMath.h>
#include <string_view>

#include <wrl/client.h>
export module ModelManager;

using Microsoft::WRL::ComPtr;

import Model;
import Vertex;

export namespace Umi
{
	struct Mesh
	{
		ComPtr<ID3D12Resource> vertexBuffer = nullptr;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
		ComPtr<ID3D12Resource> indexBuffer = nullptr;
		D3D12_INDEX_BUFFER_VIEW _ibView = {};
		ComPtr<ID3D12Resource> texture = nullptr;
		UINT indexCount = 0;
		DirectX::XMFLOAT4 diffuseColor;
		bool hasTexture = false;
	};

	//struct Mesh
	//{
	//	ComPtr<ID3D11Buffer> vertexBuffer;
	//	ComPtr<ID3D11Buffer> indexBuffer;
	//	ComPtr<ID3D11ShaderResourceView> texture;
	//	UINT indexCount = 0;
	//	DirectX::XMFLOAT4 diffuseColor;
	//	bool hasTexture = false;
	//};

	class ModelManager
	{
	private:

	public:
		[[nodiscard]] ModelID LoadModel(std::string_view filePath);

		ModelManager() = default;
		~ModelManager() = default;
	};
}
