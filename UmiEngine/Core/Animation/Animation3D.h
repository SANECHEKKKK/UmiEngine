#pragma once
#include <vector>
#include <string>

#include <DirectXMath.h>

namespace Umi
{
	struct BoneInfo 
	{
		std::string name;
		DirectX::XMMATRIX offsetMatrix;
		int parentIndex;
	};

	struct KeyPosition 
	{
		DirectX::XMFLOAT3 position;
		float timeStamp;
	};

	struct KeyRotation 
	{
		DirectX::XMFLOAT4 orientation;
		float timeStamp;
	};

	struct KeyScale 
	{
		DirectX::XMFLOAT3 scale;
		float timeStamp;
	};

	struct BoneAnimation 
	{
		std::vector<KeyPosition> positions;
		std::vector<KeyRotation> rotations;
		std::vector<KeyScale> scales;
		int boneIndex;
	};

	struct Animation3D 
	{
		std::string name;
		float duration;
		float ticksPerSecond;
		std::vector<BoneAnimation> boneAnimations;
	};
}