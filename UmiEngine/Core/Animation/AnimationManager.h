#pragma once

#include <string>

#include <Animation/AnimationState.h>
#include <State/StateComponent.h>

namespace DirectX
{
	struct XMMATRIX;
}

namespace Umi
{
	class Registry;
	struct Animator3DComponent;
	struct Model;
	struct ModelComponent;
	class ModelManager;

	class AnimationManager
	{
	private:
		Registry& registry;

		ModelManager& modelManager;

		inline void AnimationUpdate(Animator3DComponent& animator, Model& model);

		inline void AnimationSwitch(StateComponent animation, Animator3DComponent& animator, Model& model);

		void CalculateBoneTransforms(int nodeIndex, const DirectX::XMMATRIX& parentTransform, Animator3DComponent& animator, Model& model);

		void CalculateBlendedBoneTransforms(int nodeIndex, const DirectX::XMMATRIX& parentTransform, float blendFactor, Animator3DComponent& animator, Model& model);

		void PlayAnimation(StringID animation, Model& model, Animator3DComponent& animator);

		void BlendToAnimation(StringID animation, Model& model, Animator3DComponent& animator, float _blendDuration = 0.3f);

		DirectX::XMMATRIX GetTransform(struct BoneAnimation& boneAnimation, float animationTime);

	public:
		void Update();

		AnimationManager(Registry& registry, ModelManager& modelManager);
	};
}