#include <Model/ModelManager.h>
#include <d3d11.h>
#include <Model/Model.h>
#include <Vertex/Vertex.h>

#include <fstream>

#include <DirectXTK/WICTextureLoader.h>
#include <assimp/matrix4x4.h>

using namespace Umi;

struct ModelHeader
{
	char magic[4] = { 'M','O','D','L' };
	uint32_t version = 1;
};

static inline DirectX::XMMATRIX AiToXMMATRIX(const aiMatrix4x4& m)
{
	return DirectX::XMMATRIX(
		m.a1, m.a2, m.a3, m.a4,
		m.b1, m.b2, m.b3, m.b4,
		m.c1, m.c2, m.c3, m.c4,
		m.d1, m.d2, m.d3, m.d4
	);
}

ModelManager::ModelData& ModelManager::GetModel(int id)
{
	return models[id];
}

std::vector<Mesh>& ModelManager::GetMesh(int modelID)
{
	return models[modelID].m_model.meshes;
}

int ModelManager::LoadModel(const std::string& filepath)
{
	if (modelCount >= MODEL_MAX)
		return INVALID_MODEL_ID;

	LoadableModelData data;

	for (int i = 0; i < MODEL_MAX; ++i)
	{
		if (models[i].m_directory == filepath)
		{
			return i;
		}
	}

	models[modelCount].m_directory = filepath;

	std::ifstream inFile(filepath, std::ios::binary);
	if (inFile.is_open())
	{
		ModelHeader header;
		inFile.read(reinterpret_cast<char*>(&header), sizeof(ModelHeader));

		//Basic security check
		if (header.magic[0] != 'M' || header.magic[1] != 'O' ||
			header.magic[2] != 'D' || header.magic[3] != 'L') {
			inFile.close();
			return -1;
		}

		if (header.version != 1) {
			inFile.close();
			return -1;
		}

		data = ReadModelData(inFile);
		inFile.close();
	}
	else
	{
		return -1;
	}

	data.m_directory = filepath;
	data.modelID = modelCount;

	loadList.push_back(data);

	return modelCount++;
}

LoadableModelData ModelManager::ReadModelData(std::ifstream& file)
{
	LoadableModelData data;

	auto ReadString = [&file]() -> std::string {
		uint32_t length;
		file.read(reinterpret_cast<char*>(&length), sizeof(uint32_t));
		if (length > 0) {
			std::string str(length, '\0');
			file.read(&str[0], length);
			return str;
		}
		return "";
		};

	//--------------Global Inverse Transform----------------//
	file.read(reinterpret_cast<char*>(&data.globalInverseTransform), sizeof(DirectX::XMFLOAT4X4));
	//------------------------------------------------------//

	//------------------Root Node Index---------------------//
	file.read(reinterpret_cast<char*>(&data.rootNodeIndex), sizeof(int));
	//------------------------------------------------------//

	//---------------------Materials------------------------//
	uint32_t materialCount;
	file.read(reinterpret_cast<char*>(&materialCount), sizeof(uint32_t));
	data.materials.resize(materialCount);
	for (auto& material : data.materials) 
	{
		file.read(reinterpret_cast<char*>(&material.diffuseColor), sizeof(DirectX::XMFLOAT4));
		material.texturePath = ReadString();
		file.read(reinterpret_cast<char*>(&material.isEmbedded), sizeof(bool));
		file.read(reinterpret_cast<char*>(&material.textureWidth), sizeof(unsigned int));
		file.read(reinterpret_cast<char*>(&material.textureHeight), sizeof(unsigned int));

		uint32_t textureDataSize;
		file.read(reinterpret_cast<char*>(&textureDataSize), sizeof(uint32_t));
		if (textureDataSize > 0) {
			material.embeddedTextureData.resize(textureDataSize);
			file.read(reinterpret_cast<char*>(material.embeddedTextureData.data()), textureDataSize);
		}
	}
	//------------------------------------------------------//

	//---------------------Meshes---------------------------//
	uint32_t meshCount;
	file.read(reinterpret_cast<char*>(&meshCount), sizeof(uint32_t));
	data.meshes.resize(meshCount);
	for (auto& mesh : data.meshes) 
	{
		//material index
		file.read(reinterpret_cast<char*>(&mesh.materialIndex), sizeof(int));

		//vertices
		uint32_t vertexCount;
		file.read(reinterpret_cast<char*>(&vertexCount), sizeof(uint32_t));
		mesh.vertices.resize(vertexCount);
		file.read(reinterpret_cast<char*>(mesh.vertices.data()), vertexCount * sizeof(Umi::Vertex3D));

		//indices
		uint32_t indexCount;
		file.read(reinterpret_cast<char*>(&indexCount), sizeof(uint32_t));
		mesh.indices.resize(indexCount);
		file.read(reinterpret_cast<char*>(mesh.indices.data()), indexCount * sizeof(UINT));
	}
	//------------------------------------------------------//

	//---------------------Bones----------------------------//
	uint32_t boneCount;
	file.read(reinterpret_cast<char*>(&boneCount), sizeof(uint32_t));
	data.bones.resize(boneCount);
	for (auto& bone : data.bones) 
	{
		bone.name = ReadString();
		file.read(reinterpret_cast<char*>(&bone.offsetMatrix), sizeof(DirectX::XMMATRIX));
		file.read(reinterpret_cast<char*>(&bone.parentIndex), sizeof(int));
	}
	//------------------------------------------------------//
	
	//----------------Bone Name to Index Map----------------//
	uint32_t boneMapCount;
	file.read(reinterpret_cast<char*>(&boneMapCount), sizeof(uint32_t));
	for (uint32_t i = 0; i < boneMapCount; i++) 
	{
		std::string name = ReadString();
		int index;
		file.read(reinterpret_cast<char*>(&index), sizeof(int));
		data.boneNameToIndex[name] = index;
	}
	//------------------------------------------------------//

	//---------------------Animations-----------------------//
	uint32_t animationCount;
	file.read(reinterpret_cast<char*>(&animationCount), sizeof(uint32_t));
	data.animations.resize(animationCount);
	for (auto& animation : data.animations) 
	{
		animation.name = ReadString();
		file.read(reinterpret_cast<char*>(&animation.duration), sizeof(float));
		file.read(reinterpret_cast<char*>(&animation.ticksPerSecond), sizeof(float));

		//bone animations
		uint32_t boneAnimCount;
		file.read(reinterpret_cast<char*>(&boneAnimCount), sizeof(uint32_t));
		animation.boneAnimations.resize(boneAnimCount);
		for (auto& boneAnim : animation.boneAnimations) {
			file.read(reinterpret_cast<char*>(&boneAnim.boneIndex), sizeof(int));

			//positions
			uint32_t posCount;
			file.read(reinterpret_cast<char*>(&posCount), sizeof(uint32_t));
			boneAnim.positions.resize(posCount);
			file.read(reinterpret_cast<char*>(boneAnim.positions.data()), posCount * sizeof(KeyPosition));

			//rotations
			uint32_t rotCount;
			file.read(reinterpret_cast<char*>(&rotCount), sizeof(uint32_t));
			boneAnim.rotations.resize(rotCount);
			file.read(reinterpret_cast<char*>(boneAnim.rotations.data()), rotCount * sizeof(KeyRotation));

			//scales
			uint32_t scaleCount;
			file.read(reinterpret_cast<char*>(&scaleCount), sizeof(uint32_t));
			boneAnim.scales.resize(scaleCount);
			file.read(reinterpret_cast<char*>(boneAnim.scales.data()), scaleCount * sizeof(KeyScale));
		}
	}
	//------------------------------------------------------//

	//---------------------Model Nodes----------------------//
	uint32_t nodeCount;
	file.read(reinterpret_cast<char*>(&nodeCount), sizeof(uint32_t));
	data.nodes.resize(nodeCount);
	for (auto& node : data.nodes) 
	{
		node.name = ReadString();
		file.read(reinterpret_cast<char*>(&node.transformation), sizeof(DirectX::XMFLOAT4X4));
		file.read(reinterpret_cast<char*>(&node.parentIndex), sizeof(int));

		//children indices
		uint32_t childCount;
		file.read(reinterpret_cast<char*>(&childCount), sizeof(uint32_t));
		node.childrenIndices.resize(childCount);
		file.read(reinterpret_cast<char*>(node.childrenIndices.data()), childCount * sizeof(int));
	}
	//------------------------------------------------------//

	return data;
}

void ModelManager::LoadMain()
{
	for (auto& data : loadList)
	{
		ModelData modelData;

		for (auto& mesh : data.meshes)
		{
			D3D11_BUFFER_DESC vbd{};
			vbd.Usage = D3D11_USAGE_DEFAULT;
			vbd.ByteWidth = sizeof(Umi::Vertex3D) * static_cast<UINT>(mesh.vertices.size());
			vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA vinitData{};
			vinitData.pSysMem = mesh.vertices.data();

			Umi::Mesh newMesh{};
			HRESULT hr = g_Device->CreateBuffer(&vbd, &vinitData, &newMesh.vertexBuffer);
			if (FAILED(hr))
			{
				MessageBoxA(nullptr, "Failed to create vertex buffer!", "ERROR", MB_OK);
			}

			D3D11_BUFFER_DESC ibd{};
			ibd.Usage = D3D11_USAGE_DEFAULT;
			ibd.ByteWidth = sizeof(UINT) * static_cast<UINT>(mesh.indices.size());
			ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA iinitData{};
			iinitData.pSysMem = mesh.indices.data();

			hr = g_Device->CreateBuffer(&ibd, &iinitData, &newMesh.indexBuffer);
			if (FAILED(hr))
			{
				MessageBoxA(nullptr, "Failed to create index buffer!", "ERROR", MB_OK);
			}

			newMesh.indexCount = static_cast<UINT>(mesh.indices.size());

			newMesh.hasTexture = false;
			newMesh.diffuseColor = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);

			if (mesh.materialIndex >= 0 && mesh.materialIndex < static_cast<int>(data.materials.size()))
			{
				MaterialData& matData = data.materials[mesh.materialIndex];
				newMesh.diffuseColor = matData.diffuseColor;

				if (!matData.texturePath.empty())
				{
					if (matData.isEmbedded)
					{
						if (matData.textureHeight == 0)
						{
							hr = DirectX::CreateWICTextureFromMemory(
								g_Device,
								matData.embeddedTextureData.data(),
								matData.embeddedTextureData.size(),
								nullptr,
								newMesh.texture.GetAddressOf()
							);
						}
						else
						{
							D3D11_TEXTURE2D_DESC texDesc = {};
							texDesc.Width = matData.textureWidth;
							texDesc.Height = matData.textureHeight;
							texDesc.MipLevels = 1;
							texDesc.ArraySize = 1;
							texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
							texDesc.SampleDesc.Count = 1;
							texDesc.Usage = D3D11_USAGE_DEFAULT;
							texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

							D3D11_SUBRESOURCE_DATA initData = {};
							initData.pSysMem = matData.embeddedTextureData.data();
							initData.SysMemPitch = matData.textureWidth * 4;

							Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
							hr = g_Device->CreateTexture2D(&texDesc, &initData, tex2D.GetAddressOf());

							if (SUCCEEDED(hr))
							{
								hr = g_Device->CreateShaderResourceView(
									tex2D.Get(),
									nullptr,
									newMesh.texture.GetAddressOf()
								);
							}
						}

						if (FAILED(hr))
						{
							MessageBoxA(nullptr, "Failed to load embedded texture!", "WARNING", MB_OK);
						}
					}
					else
					{
						std::string fullPath = data.m_directory + "\\" + matData.texturePath;

						hr = DirectX::CreateWICTextureFromFileEx(
							g_Device,
							g_DeviceContext,
							std::wstring(fullPath.begin(), fullPath.end()).c_str(),
							0,
							D3D11_USAGE_DEFAULT,
							D3D11_BIND_SHADER_RESOURCE,
							0,
							D3D11_RESOURCE_MISC_GENERATE_MIPS,
							DirectX::WIC_LOADER_DEFAULT,
							nullptr,
							newMesh.texture.GetAddressOf()
						);

						if (FAILED(hr))
						{
							MessageBoxA(nullptr, ("Failed to load texture: " + matData.texturePath).c_str(), "WARNING", MB_OK);
						}
					}

					if (SUCCEEDED(hr))
					{
						newMesh.hasTexture = true;
					}
				}
			}

			modelData.m_model.meshes.push_back(newMesh);
		}

		modelData.m_model.bones = data.bones;
		modelData.m_model.animations = data.animations;
		modelData.m_model.boneNameToIndex = data.boneNameToIndex;
		modelData.m_model.globalInverseTransform = data.globalInverseTransform;
		modelData.m_model.nodes = data.nodes;
		modelData.m_model.rootNodeIndex = data.rootNodeIndex;
		modelData.id = data.modelID;

		models[data.modelID] = modelData;
	}
}

ModelManager::ModelManager(ID3D11Device* _g_Device, ID3D11DeviceContext* _g_DeviceContext)
{
	g_Device = _g_Device;
	g_DeviceContext = _g_DeviceContext;
}
