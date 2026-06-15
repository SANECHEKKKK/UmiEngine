module;
#include <DirectXMath.h>
#include <string_view>
#include <vector>

#include <Graphics/d3dx12.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <wrl/client.h>

#include <EngineApi/EngineApi.h>
export module ModelManager;

using Microsoft::WRL::ComPtr;

import Model;
import Vertex;
import GraphicsContext;
import TextureManager;
import Vertex;
import Material;

export namespace Umi
{
	struct ENGINE_API Mesh
	{
		ComPtr<ID3D12Resource> vertexBuffer = nullptr;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
		std::vector<Vertex3D> vertices;
		
		ComPtr<ID3D12Resource> indexBuffer = nullptr;
		D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
		UINT indexCount = 0;
		std::vector<UINT> indices;

		UINT materialIndex = 0;
	};

	struct ENGINE_API ModelData
	{
		std::vector<Mesh> meshes;
		std::vector<Material> materials;
		std::string filePath;
	};

	class ENGINE_API ModelManager
	{
	private:
		TextureManager& textureManager;
		GraphicsContext& graphicsContext;

		std::vector<ModelData> modelList;
		
		std::vector<ComPtr<ID3D12Resource>> textureResources;

		void LoadNode(const aiNode* node, const aiScene* scene, ModelData& modelData);
		void LoadMesh(const aiMesh* mesh, const aiScene* scene, ModelData& modelData);
		void LoadTexture(const aiScene* scene, ModelData& modelData);

		bool CreateVertexBuffer(Mesh& mesh);
		bool CreateIndexBuffer(Mesh& mesh);

	public:
		[[nodiscard]] ModelID LoadModel(std::string_view filePath);
		ModelData& GetModelData(ModelID id);

		ModelManager(TextureManager& textureManager, GraphicsContext& graphicsContext);
		~ModelManager() = default;
	};
}
