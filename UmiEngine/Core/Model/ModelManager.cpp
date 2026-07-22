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
import Error;

using namespace Umi;
using Microsoft::WRL::ComPtr;

static DirectX::XMFLOAT4X4 ToXM(const aiMatrix4x4& m)
{
	return DirectX::XMFLOAT4X4(
		m.a1, m.b1, m.c1, m.d1,
		m.a2, m.b2, m.c2, m.d2,
		m.a3, m.b3, m.c3, m.d3,
		m.a4, m.b4, m.c4, m.d4);
}

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

	LoadBones(mesh, meshData, modelData);

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
		material.baseColor.x = color.r;
		material.baseColor.y = color.g;
		material.baseColor.z = color.b;
		material.baseColor.w = 1.0f;

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

void ModelManager::BuildNodeHierarchy(const aiNode* node, int parentIndex, ModelData& modelData)
{
	const int index = static_cast<int>(modelData.skeleton.nodes.size());

	ModelNode entry;
	entry.name = node->mName.C_Str();
	entry.localTransform = ToXM(node->mTransformation);
	entry.parentIndex = parentIndex;
	modelData.skeleton.nodes.push_back(std::move(entry));
	modelData.skeleton.nodeNameToIndex[node->mName.C_Str()] = index;

	for (unsigned i = 0; i < node->mNumChildren; ++i)
		BuildNodeHierarchy(node->mChildren[i], index, modelData);
}

void ModelManager::LoadBones(const aiMesh* mesh, Mesh& out, ModelData& modelData)
{
	if (!mesh->HasBones()) return;

	auto& skeleton = modelData.skeleton;

	for (auto& v : out.vertices)
		for (int i = 0; i < 4; ++i) v.boneIDs[i] = -1;

	for (unsigned b = 0; b < mesh->mNumBones; ++b)
	{
		const aiBone* aibone = mesh->mBones[b];
		const std::string name = aibone->mName.C_Str();

		int boneIndex;
		auto it = skeleton.boneNameToIndex.find(name);
		if (it != skeleton.boneNameToIndex.end())
		{
			boneIndex = it->second;
		}
		else
		{
			boneIndex = static_cast<int>(skeleton.bones.size());
			if (boneIndex >= MAX_BONES)
			{
				Error::NonFatalError("Model exceeds MAX_BONES: " + modelData.filePath);
				return;
			}

			BoneInfo info;
			info.offsetMatrix = ToXM(aibone->mOffsetMatrix);

			auto node = skeleton.nodeNameToIndex.find(name);
			info.nodeIndex = (node != skeleton.nodeNameToIndex.end()) ? node->second : -1;
			if (info.nodeIndex < 0) continue;

			skeleton.bones.push_back(info);
			skeleton.boneNameToIndex[name] = boneIndex;
		}

		for (unsigned w = 0; w < aibone->mNumWeights; ++w)
		{
			const aiVertexWeight& vw = aibone->mWeights[w];
			if (vw.mVertexId >= out.vertices.size()) continue;

			Vertex3D& v = out.vertices[vw.mVertexId];
			for (int slot = 0; slot < 4; ++slot)
			{
				if (v.boneWeights[slot] == 0.0f)
				{
					v.boneIDs[slot] = boneIndex;
					v.boneWeights[slot] = vw.mWeight;
					break;
				}
			}
		}
	}
}

void ModelManager::LoadAnimations(const aiScene* scene, ModelData& modelData)
{
	const auto& skeleton = modelData.skeleton;

	for (unsigned a = 0; a < scene->mNumAnimations; ++a)
	{
		const aiAnimation* src = scene->mAnimations[a];

		AnimationClip clip;
		clip.name = src->mName.C_Str();
		clip.duration = static_cast<float>(src->mDuration);
		clip.ticksPerSecond = src->mTicksPerSecond > 0.0
			? static_cast<float>(src->mTicksPerSecond) : 25.0f;
		clip.nodeToChannel.assign(skeleton.nodes.size(), -1);

		for (unsigned c = 0; c < src->mNumChannels; ++c)
		{
			const aiNodeAnim* src_ch = src->mChannels[c];

			auto it = skeleton.nodeNameToIndex.find(src_ch->mNodeName.C_Str());
			if (it == skeleton.nodeNameToIndex.end()) continue;

			NodeChannel ch;
			ch.nodeIndex = it->second;

			ch.positions.reserve(src_ch->mNumPositionKeys);
			for (unsigned k = 0; k < src_ch->mNumPositionKeys; ++k)
			{
				const auto& key = src_ch->mPositionKeys[k];
				ch.positions.push_back({ { key.mValue.x, key.mValue.y, key.mValue.z },
										 static_cast<float>(key.mTime) });
			}

			ch.rotations.reserve(src_ch->mNumRotationKeys);
			for (unsigned k = 0; k < src_ch->mNumRotationKeys; ++k)
			{
				const auto& key = src_ch->mRotationKeys[k];
				ch.rotations.push_back({ { key.mValue.x, key.mValue.y,
										   key.mValue.z, key.mValue.w },
										 static_cast<float>(key.mTime) });
			}

			ch.scales.reserve(src_ch->mNumScalingKeys);
			for (unsigned k = 0; k < src_ch->mNumScalingKeys; ++k)
			{
				const auto& key = src_ch->mScalingKeys[k];
				ch.scales.push_back({ { key.mValue.x, key.mValue.y, key.mValue.z },
									  static_cast<float>(key.mTime) });
			}

			clip.nodeToChannel[ch.nodeIndex] = static_cast<int>(clip.channels.size());
			clip.channels.push_back(std::move(ch));
		}

		modelData.animations.push_back(std::move(clip));
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
		Error::NonFatalError("Failed to create vertex buffer for mesh.");
		return false;
	}

	Vertex3D* vertMapped = nullptr;

	result = mesh.vertexBuffer->Map(0, nullptr, (void**)&vertMapped);
	if (result != S_OK)
	{
		Error::NonFatalError("Failed to map vertex buffer for mesh.");
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
		Error::NonFatalError("Failed to create index buffer for mesh.");
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
		aiProcess_EmbedTextures	|
		aiProcess_LimitBoneWeights
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
		Error::NonFatalError("Assimp failed to load model: " + std::string(importer.GetErrorString()));
		return INVALID_MODELID;
	}
	//----------------------------------------------------
	
	modelData.skeleton.nodes.reserve(256);
	BuildNodeHierarchy(scene->mRootNode, -1, modelData);
	modelData.skeleton.rootNodeIndex = 0;

	aiMatrix4x4 globalInverse = scene->mRootNode->mTransformation;
	globalInverse.Inverse();
	modelData.skeleton.globalInverseTransform = ToXM(globalInverse);

	//--------Recursively process the root node and its children--------
	LoadNode(scene->mRootNode, scene, modelData);
	//------------------------------------------------------------------
	
	LoadAnimations(scene, modelData);

	LoadTexture(scene, modelData);

	for (auto& mesh : modelData.meshes)
	{
		if (!CreateVertexBuffer(mesh) || !CreateIndexBuffer(mesh))
		{
			Error::NonFatalError("Failed to create buffers for mesh.");
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
		Error::NonFatalError("Invalid model ID.");
		return modelList[0];
	}
	return modelList[static_cast<size_t>(id)];
}

ModelManager::ModelManager(TextureManager& textureManager, GraphicsContext& graphicsContext) : textureManager(textureManager), graphicsContext(graphicsContext)
{
	modelList.reserve(64);
}