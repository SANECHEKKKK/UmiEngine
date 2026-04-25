#pragma once
#include <vector>
#include <string>
#include <array>

#include <Model/Model.h>

struct aiNode;
struct aiScene;
struct aiMesh;
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace Umi
{
	struct Vertex3D;
	struct ModelNode;
	struct Mesh;

	class ModelManager
	{
	private:
		struct ModelData
		{
			int id = -1;
			Umi::Model m_model;
			std::string m_directory;
		};

		std::vector<Umi::LoadableModelData> loadList;

		ID3D11Device* g_Device = nullptr;
		ID3D11DeviceContext* g_DeviceContext = nullptr;

		static constexpr int MODEL_MAX = 100;
		std::array<ModelData, MODEL_MAX> models;
		int modelCount = 0;

		static constexpr int INVALID_MODEL_ID = -1;

	public:
		ModelData& GetModel(int id);
		std::vector<Umi::Mesh>& GetMesh(int modelID);

		int LoadModel(const std::string& filepath);
		Umi::LoadableModelData ReadModelData(std::ifstream& file);
		void LoadMain();

		ModelManager(ID3D11Device* _g_Device, ID3D11DeviceContext* _g_DeviceContext);
		ModelManager(const ModelManager&) = delete;
		ModelManager& operator=(const ModelManager&) = delete;
		ModelManager(ModelManager&&) = delete;
		ModelManager& operator=(ModelManager&&) = delete;

		~ModelManager() = default;
	};
}