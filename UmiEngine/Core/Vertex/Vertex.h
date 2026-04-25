#pragma once
#include <DirectXMath.h>

namespace Umi
{
	struct Vertex2D
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
	};

	struct Vertex3D
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
		int boneIDs[4];
		float boneWeights[4];
	};

}