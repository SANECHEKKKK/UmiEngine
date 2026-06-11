module;
#include <DirectXMath.h>

#include <EngineApi/EngineApi.h>
export module Vertex;

export namespace Umi
{
	struct ENGINE_API Vertex2D
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
	};

	struct ENGINE_API Vertex3D
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
		int boneIDs[4];
		float boneWeights[4];
	};

}