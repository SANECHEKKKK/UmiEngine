module;
#include <DirectXMath.h>

#include <vector>

#include <EngineApi/EngineApi.h>
export module AnimationManager;

import Registry;
import ModelManager;
import Animation;
import Animator;

export namespace Umi
{
	class ENGINE_API AnimationManager
	{
	private:
		Registry& registry;
		ModelManager& modelManager;

		std::vector<DirectX::XMVECTOR> localT, localR, localS;
		std::vector<DirectX::XMMATRIX> globals;

		void ResolveSlots(Animator& animator, const Skeleton& skeleton,
		                  const std::vector<AnimationClip>& clips);

		void ApplyPending(Animator& animator);

		void AdvanceTime(Animator& animator,
		                 const std::vector<AnimationClip>& clips,
		                 float deltaTime);

		void SampleClip(const AnimationClip& clip, const Skeleton& skeleton,
		                float timeSeconds, bool blendInto, float weight);

		void BuildPalette(Animator& animator, const Skeleton& skeleton);

	public:
		void Update(float deltaTime);

		AnimationManager(Registry& registry, ModelManager& modelManager);
	};
}
