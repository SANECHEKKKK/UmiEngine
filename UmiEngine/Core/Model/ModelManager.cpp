module;
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Graphics/d3dx12.h>

#define STB_IMAGE_IMPLEMENTATION
#include <Texture/stb_image.h>

#include <DirectXMath.h>

#include <string_view>

#include <wrl/client.h>
module ModelManager;

import Model;
import Vertex;
import GraphicsContext;
import EngineContext;

using namespace Umi;
using Microsoft::WRL::ComPtr;

void ModelManager::LoadNode(const aiNode* node, const aiScene* scene, ModelData& modelData)
{
	//--------Process all the meshes of the current node--------
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		LoadMesh(mesh, scene, modelData);
	}
	//----------------------------------------------------------

	//--------Recursively process all the children nodes--------
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		LoadNode(node->mChildren[i], scene, modelData);
	}
	//----------------------------------------------------------
}

void ModelManager::LoadMesh(const aiMesh* mesh, const aiScene* scene, ModelData& modelData)
{
	Mesh meshData{};

	//------------VERTEX DATA EXTRACTION-------------
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex3D vertex{};

		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		if (mesh->HasNormals())
		{
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}
		else
		{
			vertex.normal = DirectX::XMFLOAT3(0, 1, 0);
		}

		if (mesh->mTextureCoords[0])
		{
			vertex.texcoord.x = mesh->mTextureCoords[0][i].x;
			vertex.texcoord.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.texcoord = DirectX::XMFLOAT2(0.0f, 0.0f);
		}

		meshData.vertices.push_back(vertex);
	}
	//-----------------------------------------------

	//-------------INDEX DATA EXTRACTION-------------
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++)
			meshData.indices.push_back(face.mIndices[j]);
	}
	meshData.indexCount = static_cast<UINT>(meshData.indices.size());
	//-----------------------------------------------

	//-----------MATERIAL INDEX EXTRACTION-----------
	meshData.materialIndex = mesh->mMaterialIndex;
	//-----------------------------------------------

	modelData.meshes.push_back(meshData);
}

void ModelManager::LoadTexture(const aiScene* scene, ModelData& modelData)
{
	for (UINT i = 0; i < scene->mNumMaterials; i++)
	{
		Material material;

		aiMaterial* aimaterial = scene->mMaterials[i];


		// Copy diffuse color
		aiColor3D color(0.8f, 0.8f, 0.8f);
		aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material.baseColor[0] = color.r;
		material.baseColor[1] = color.g;
		material.baseColor[2] = color.b;
		material.baseColor[3] = 1.0f;

		aiString texPath;
		bool textureFound = false;

		// Try different texture types
		aiTextureType textureTypes[] = {
			aiTextureType_DIFFUSE,
			aiTextureType_BASE_COLOR,
			aiTextureType_UNKNOWN,
			aiTextureType_NONE,
			aiTextureType_EMISSIVE,
			aiTextureType_SPECULAR,
			aiTextureType_AMBIENT,
			aiTextureType_REFLECTION,
			aiTextureType_NORMALS,
			aiTextureType_HEIGHT,
			aiTextureType_METALNESS,
			aiTextureType_DIFFUSE_ROUGHNESS,
		};

		for (aiTextureType type : textureTypes)
		{
			if (aimaterial->GetTextureCount(type) > 0)
			{
				if (aimaterial->GetTexture(type, 0, &texPath) == AI_SUCCESS)
				{
					textureFound = true;
					break;
				}
			}
		}

		if (!textureFound)
		{
			const char* propertyKeys[] = {
		   "$tex.file",
		   "DiffuseColor",
		   "$raw.DiffuseColor",
		   "Maya|TEX_color_map",
		   "Maya|file",
		   "$clr.diffuse",
		   "baseColor"
			};

			for (const char* key : propertyKeys)
			{
				if (aiGetMaterialString(aimaterial, key, 0, 0, &texPath) == AI_SUCCESS)
				{
					textureFound = true;
					break;
				}
			}
		}

		if (textureFound)
		{
			std::string texPathStr = texPath.C_Str();
			std::vector<uint8_t> embeddedTextureData;
			int width, height, channels;

			if (!texPathStr.empty() && texPathStr[0] == '*')
			{
				int texIndex = atoi(texPathStr.c_str() + 1);

				if (texIndex >= 0 && texIndex < static_cast<int>(scene->mNumTextures))
				{
					aiTexture* embeddedTex = scene->mTextures[texIndex];

					embeddedTextureData.resize(embeddedTex->mWidth);
					memcpy(embeddedTextureData.data(), embeddedTex->pcData, embeddedTex->mWidth);

					if (embeddedTex->mHeight == 0) // Compressed format
					{
						uint8_t* decoded = stbi_load_from_memory(
							embeddedTextureData.data(),
							embeddedTextureData.size(),
							&width, &height, &channels,
							4  // force RGBA
						);

						if (decoded) {
							textureManager.LoadTexture3DRawData(decoded, width, height);
							material.albedoIndex = graphicsContext.descriptorHeap3D.Size() - 2;
							modelData.materials.push_back(material);
							stbi_image_free(decoded);
						}
						else
						{
							// Handle decoding failure
							width = height = 0;
						}

					}
					else // Uncompressed ARGB8888
					{
						width = embeddedTex->mWidth;
						height = embeddedTex->mHeight;

						std::vector<uint8_t> pixels(width * height * 4);

						for (size_t i = 0; i < width * height; i++) {
							aiTexel& t = embeddedTex->pcData[i];
							pixels[i * 4 + 0] = t.r;
							pixels[i * 4 + 1] = t.g;
							pixels[i * 4 + 2] = t.b;
							pixels[i * 4 + 3] = t.a;
						}

						textureManager.LoadTexture3DRawData(pixels.data(), width, height);
						material.albedoIndex = graphicsContext.descriptorHeap3D.Size() - 2;
						modelData.materials.push_back(material);
					}
				}
			}

			////textureManager.LogInfo("Texture path: " + std::string(texPath.C_Str()));
		}
		else
		{
			modelData.materials.push_back(material);
		}
	}
}

bool ModelManager::CreateVertexBuffer(Mesh& mesh)
{
	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(mesh.vertices.size() * sizeof(Vertex3D));

	auto result = graphicsContext.device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mesh.vertexBuffer)
	);
	if (result != S_OK)
	{
		// Handle non-fatal error
		return false;
	}

	Vertex3D* vertMapped = nullptr;

	result = mesh.vertexBuffer->Map(0, nullptr, (void**)&vertMapped);
	if (result != S_OK)
	{
		// Handle non-fatal error
		return false;
	}

	std::copy(std::begin(mesh.vertices), std::end(mesh.vertices), vertMapped);

	mesh.vertexBuffer->Unmap(0, nullptr);

	mesh.vertexBufferView.BufferLocation = mesh.vertexBuffer->GetGPUVirtualAddress();
	mesh.vertexBufferView.SizeInBytes = mesh.vertices.size() * sizeof(Vertex3D);
	mesh.vertexBufferView.StrideInBytes = sizeof(Vertex3D);

	return true;
}

bool ModelManager::CreateIndexBuffer(Mesh& mesh)
{
	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(mesh.indices.size() * sizeof(UINT));

	auto result = graphicsContext.device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mesh.indexBuffer)
	);
	if (result != S_OK)
	{
		// Handle non-fatal error
		return false;
	}

	UINT* mappedIdx = nullptr;

	mesh.indexBuffer->Map(0, nullptr, (void**)&mappedIdx);
	std::copy(std::begin(mesh.indices), std::end(mesh.indices), mappedIdx);
	mesh.indexBuffer->Unmap(0, nullptr);

	mesh.indexBufferView.BufferLocation = mesh.indexBuffer->GetGPUVirtualAddress();
	mesh.indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	mesh.indexBufferView.SizeInBytes = mesh.indices.size() * sizeof(mesh.indices[0]);

	return true;
}

ModelID ModelManager::LoadModel(std::string_view filePath)
{
	//--------Check if the model is already loaded--------
	for (const auto& model : modelList)
	{
		if (model.filePath == filePath)
			return ModelID(&model - &modelList[0]);
	}
	//----------------------------------------------------

	//-------------Load the model using Assimp------------
	ModelData modelData{};
	modelData.filePath = std::string(filePath);

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(
		filePath.data(),
		aiProcess_Triangulate |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_EmbedTextures

		//aiProcess_Triangulate |
		//aiProcess_ConvertToLeftHanded |
		//aiProcess_CalcTangentSpace |
		//aiProcess_GenNormals |
		//aiProcess_JoinIdenticalVertices |  // Optimize vertex data
		//aiProcess_OptimizeMeshes |
		//aiProcess_FlipUVs |              // Sometimes needed
		//aiProcess_EmbedTextures
	);

	if (!scene || !scene->HasMeshes() || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		// Handle non fatal error
		return INVALID_MODELID;
	}
	//----------------------------------------------------

	//--------Recursively process the root node and its children--------
	LoadNode(scene->mRootNode, scene, modelData);
	//------------------------------------------------------------------

	LoadTexture(scene, modelData);

	for (auto& mesh : modelData.meshes)
	{
		if (!CreateVertexBuffer(mesh) || !CreateIndexBuffer(mesh))
		{
			// Handle non fatal error
			return INVALID_MODELID;
		}
	}

	modelList.push_back(modelData);

	return ModelID(modelList.size() - 1);
}

ModelData& ModelManager::GetModelData(ModelID id)
{
	if (static_cast<size_t>(id) >= modelList.size())
	{
		// Handle non fatal error
		return modelList[0]; // Return a default model data or handle it as needed
	}
	return modelList[static_cast<size_t>(id)];
}

ModelManager::ModelManager(TextureManager& textureManager, GraphicsContext& graphicsContext) : textureManager(textureManager), graphicsContext(graphicsContext)
{
	modelList.reserve(64);
}