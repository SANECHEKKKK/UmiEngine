module;
#include <DirectXMath.h>
#include <string_view>
#include <vector>

#include <Graphics/d3dx12.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include <wrl/client.h>
export module ModelManager;

using Microsoft::WRL::ComPtr;

import Model;
import Vertex;
import GraphicsContext;
import Vertex;

export namespace Umi
{
	struct Mesh
	{
		ComPtr<ID3D12Resource> vertexBuffer = nullptr;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
		std::vector<Vertex3D> vertices;
		
		ComPtr<ID3D12Resource> indexBuffer = nullptr;
		D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
		UINT indexCount = 0;
		std::vector<UINT> indices;
		
		ComPtr<ID3D12Resource> texture = nullptr;
		DirectX::XMFLOAT4 diffuseColor;
		bool hasTexture = false;
	};

	struct ModelData
	{
		std::vector<Mesh> meshes;
		std::string filePath;
	};

	class ModelManager
	{
	private:
		GraphicsContext& graphicsContext;

		std::vector<ModelData> modelList;

		void LoadNode(const aiNode* node, const aiScene* scene, ModelData& modelData);
		void LoadMesh(const aiMesh* mesh, const aiScene* scene, ModelData& modelData);

		bool CreateVertexBuffer(Mesh& mesh);
		bool CreateIndexBuffer(Mesh& mesh);

	public:
		[[nodiscard]] ModelID LoadModel(std::string_view filePath);
		ModelData& GetModelData(ModelID id);

		ModelManager(GraphicsContext& graphicsContext);
		~ModelManager() = default;
	};
}
