#pragma once
#include <string>
#include <vector>
#include <map>

#include <DirectXMath.h>
#include <Vertex/Vertex.h>
#include <Animation/Animation3D.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11Buffer;
struct ID3D11ShaderResourceView;

namespace Umi
{
	struct Mesh 
	{
		ComPtr<ID3D11Buffer> vertexBuffer;
		ComPtr<ID3D11Buffer> indexBuffer;
		ComPtr<ID3D11ShaderResourceView> texture;
		UINT indexCount = 0;
		DirectX::XMFLOAT4 diffuseColor;
		bool hasTexture = false;
	};

	struct ModelNode 
	{
		std::string name;
		DirectX::XMFLOAT4X4 transformation;
		int parentIndex = -1;
		std::vector<int> childrenIndices;
	};

	struct Model
	{
		std::vector<Mesh> meshes;
		std::vector<BoneInfo> bones;
		std::vector<Animation3D> animations;
		std::map<std::string, int> boneNameToIndex;
		DirectX::XMFLOAT4X4 globalInverseTransform;
		std::vector<ModelNode> nodes;
		int rootNodeIndex = 0;
	};

	struct MaterialData
	{
		DirectX::XMFLOAT4 diffuseColor = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		std::string texturePath;
		bool isEmbedded = false;
		std::vector<unsigned char> embeddedTextureData;
		unsigned int textureWidth = 0;
		unsigned int textureHeight = 0;
	};

	struct LoadableMeshData
	{
		std::vector<Vertex3D> vertices;
		std::vector<UINT> indices;
		int materialIndex = -1;
	};

	struct LoadableModelData
	{
		std::vector<LoadableMeshData> meshes;
		std::vector<MaterialData> materials;
		std::vector<BoneInfo> bones;
		std::vector<Animation3D> animations;
		std::map<std::string, int> boneNameToIndex;
		DirectX::XMFLOAT4X4 globalInverseTransform;
		std::vector<ModelNode> nodes;
		int rootNodeIndex = 0;
		std::string m_directory;
		int modelID = -1;
	};
}