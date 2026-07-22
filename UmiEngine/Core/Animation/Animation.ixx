module;
#include <DirectXMath.h>

#include <string>
#include <vector>
#include <unordered_map>

#include <EngineApi/EngineApi.h>
export module Animation;

export namespace Umi
{
	inline constexpr int MAX_BONES = 128;

	struct ENGINE_API ModelNode
	{
		std::string name;
		DirectX::XMFLOAT4X4 localTransform;
		int parentIndex = -1;                 
	};

	struct ENGINE_API BoneInfo
	{
		int nodeIndex = -1;                   
		DirectX::XMFLOAT4X4 offsetMatrix;     
	};

	struct KeyVec3 { DirectX::XMFLOAT3 value; float time; };
	struct KeyQuat { DirectX::XMFLOAT4 value; float time; };   

	struct ENGINE_API NodeChannel
	{
		int nodeIndex = -1;
		std::vector<KeyVec3> positions;
		std::vector<KeyQuat> rotations;
		std::vector<KeyVec3> scales;
	};

	struct ENGINE_API AnimationClip
	{
		std::string name;
		float duration = 0.0f;
		float ticksPerSecond = 25.0f;
		std::vector<NodeChannel> channels;

		std::vector<int> nodeToChannel;

		float DurationSeconds() const noexcept
		{
			return ticksPerSecond > 0.0f ? duration / ticksPerSecond : 0.0f;
		}
	};

	struct ENGINE_API Skeleton
	{
		std::vector<ModelNode> nodes;
		std::vector<BoneInfo>  bones;
		std::unordered_map<std::string, int> nodeNameToIndex;
		std::unordered_map<std::string, int> boneNameToIndex;
		DirectX::XMFLOAT4X4 globalInverseTransform;
		int rootNodeIndex = 0;

		bool HasSkin() const noexcept { return !bones.empty(); }
	};
}