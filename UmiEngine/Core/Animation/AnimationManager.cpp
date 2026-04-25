#include <Animation/AnimationManager.h>
#include <d3d11.h>

#include <Registry/Registry.h>
#include <Animation/Animator3DComponent.h>
#include <Model/ModelComponent.h>
#include <Model/ModelManager.h>
#include <State/StateComponent.h>
#include <Animation/Animation3D.h>
#include <Model/Model.h>
#include <Time/Time.h>

#include <DirectXMath.h>

using namespace Umi;

int FindPositionIndex(const std::vector<KeyPosition>& keys, float animationTime)
{
	for (int i = 0; i < keys.size() - 1; i++)
	{
		if (animationTime < keys[i + 1].timeStamp)
			return i;
	}
	return 0;
}

int FindRotationIndex(const std::vector<KeyRotation>& keys, float animationTime)
{
	for (int i = 0; i < keys.size() - 1; i++)
	{
		if (animationTime < keys[i + 1].timeStamp)
			return i;
	}
	return 0;
}

int FindScaleIndex(const std::vector<KeyScale>& keys, float animationTime)
{
	for (int i = 0; i < keys.size() - 1; i++)
	{
		if (animationTime < keys[i + 1].timeStamp)
			return i;
	}
	return 0;
}

float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
	float scaleFactor = 0.0f;
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	scaleFactor = midWayLength / framesDiff;
	return scaleFactor;
}

void AnimationManager::CalculateBoneTransforms(int nodeIndex, const DirectX::XMMATRIX& parentTransform, Animator3DComponent& animator, Model& model)
{
	if (nodeIndex < 0 || nodeIndex >= model.nodes.size())
		return;

	ModelNode& node = model.nodes[nodeIndex];
	const std::string& nodeName = node.name;
	DirectX::XMMATRIX nodeTransform;

	BoneAnimation* boneAnim = nullptr;
	int boneIndex = -1;

	auto boneIt = model.boneNameToIndex.find(nodeName);
	if (boneIt != model.boneNameToIndex.end())
	{
		boneIndex = boneIt->second;

		if (animator.currentAnimation)
		{
			for (auto& anim : animator.currentAnimation->boneAnimations)
			{
				if (anim.boneIndex == boneIndex)
				{
					boneAnim = &anim;
					break;
				}
			}
		}
	}

	if (boneAnim)
	{
		nodeTransform = GetTransform(*boneAnim, animator.currentTime);
	}
	else
	{
		nodeTransform = XMLoadFloat4x4(&node.transformation);
	}

	DirectX::XMMATRIX globalTransform = nodeTransform * parentTransform;

	if (boneIndex != -1)
	{
		DirectX::XMMATRIX globalInverse = XMLoadFloat4x4(&model.globalInverseTransform);
		animator.finalBoneMatrices[boneIndex] =
			model.bones[boneIndex].offsetMatrix *
			globalTransform *
			globalInverse;
	}

	for (int childIndex : node.childrenIndices)
	{
		CalculateBoneTransforms(childIndex, globalTransform, animator, model);
	}

}

void AnimationManager::CalculateBlendedBoneTransforms(int nodeIndex, const DirectX::XMMATRIX& parentTransform, float blendFactor, Animator3DComponent& animator, Model& model)
{
	if (nodeIndex < 0 || nodeIndex >= model.nodes.size())
		return;

	ModelNode& node = model.nodes[nodeIndex];
	const std::string& nodeName = node.name;
	DirectX::XMMATRIX nodeTransform;

	BoneAnimation* currentBoneAnim = nullptr;
	BoneAnimation* nextBoneAnim = nullptr;
	int boneIndex = -1;

	if (model.boneNameToIndex.find(nodeName) != model.boneNameToIndex.end())
	{
		boneIndex = model.boneNameToIndex[nodeName];

		//Find animation data for current animation
		if (animator.currentAnimation)
		{
			for (auto& anim : animator.currentAnimation->boneAnimations)
			{
				if (anim.boneIndex == boneIndex)
				{
					currentBoneAnim = &anim;
					break;
				}
			}
		}

		//Find animation data for next animation
		if (animator.nextAnimation)
		{
			for (auto& anim : animator.nextAnimation->boneAnimations)
			{
				if (anim.boneIndex == boneIndex)
				{
					nextBoneAnim = &anim;
					break;
				}
			}
		}
	}

	//Get transforms from both animations
	DirectX::XMMATRIX currentTransform;
	DirectX::XMMATRIX nextTransform;

	if (currentBoneAnim)
	{
		currentTransform = GetTransform(*currentBoneAnim, animator.currentTime);
	}
	else
	{
		currentTransform = XMLoadFloat4x4(&node.transformation);
	}

	if (nextBoneAnim)
	{
		nextTransform = GetTransform(*nextBoneAnim, animator.nextTime);
	}
	else
	{
		nextTransform = XMLoadFloat4x4(&node.transformation);
	}

	//Decompose matrices into translation, rotation, scale
	DirectX::XMVECTOR currentScale, currentRotQuat, currentTrans;
	DirectX::XMVECTOR nextScale, nextRotQuat, nextTrans;

	XMMatrixDecompose(&currentScale, &currentRotQuat, &currentTrans, currentTransform);
	XMMatrixDecompose(&nextScale, &nextRotQuat, &nextTrans, nextTransform);

	//Interpolate components
	DirectX::XMVECTOR blendedScale = DirectX::XMVectorLerp(currentScale, nextScale, blendFactor);
	DirectX::XMVECTOR blendedRotation = DirectX::XMQuaternionSlerp(currentRotQuat, nextRotQuat, blendFactor);
	DirectX::XMVECTOR blendedTranslation = DirectX::XMVectorLerp(currentTrans, nextTrans, blendFactor);

	//Reconstruct blended matrix
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(blendedScale);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(blendedRotation);
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(blendedTranslation);

	nodeTransform = scaleMatrix * rotationMatrix * translationMatrix;

	DirectX::XMMATRIX globalTransform = nodeTransform * parentTransform;

	if (boneIndex != -1)
	{
		DirectX::XMMATRIX globalInverse = XMLoadFloat4x4(&model.globalInverseTransform);
		animator.finalBoneMatrices[boneIndex] =
			model.bones[boneIndex].offsetMatrix *
			globalTransform *
			globalInverse;
	}

	for (int childIndex : node.childrenIndices)
	{
		CalculateBlendedBoneTransforms(childIndex, globalTransform, blendFactor, animator, model);
	}
}

void AnimationManager::PlayAnimation(StringID animation, Model& model, Animator3DComponent& animator)
{
	for (auto& anim : model.animations)
	{
		if (anim.name == animator.animNames[animation])
		{
			animator.currentAnimation = &anim;
			animator.currentTime = 0.0f;
			animator.isBlending = false;
			animator.nextAnimation = nullptr;

			if (animator.finalBoneMatrices.size() != model.bones.size())
			{
				animator.finalBoneMatrices.assign(model.bones.size(), DirectX::XMMatrixIdentity());
			}
			return;
		}
	}
}

void AnimationManager::BlendToAnimation(StringID animation, Model& model, Animator3DComponent& animator, float _blendDuration)
{
	if (!animator.currentAnimation)
	{
		PlayAnimation(animation, model, animator);
		return;
	}

	if (animator.nextAnimation && animator.nextAnimation->name == animator.animNames[animation])
		return;

	if (!animator.nextAnimation && animator.currentAnimation->name == animator.animNames[animation])
		return;

	for (auto& anim : model.animations)
	{
		if (anim.name == animator.animNames[animation])
		{
			animator.nextAnimation = &anim;
			animator.currentTime = 0.0f;
			animator.blendTime = 0.0f;
			animator.isBlending = true;
			animator.blendDuration = _blendDuration;
			return;
		}
	}
}\

DirectX::XMMATRIX AnimationManager::GetTransform(BoneAnimation& boneAnimation, float animationTime)
{
	DirectX::XMVECTOR position;
	if (boneAnimation.positions.size() == 1)
	{
		position = XMLoadFloat3(&boneAnimation.positions[0].position);
	}
	else
	{
		int p0Index = FindPositionIndex(boneAnimation.positions, animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(boneAnimation.positions[p0Index].timeStamp,
			boneAnimation.positions[p1Index].timeStamp,
			animationTime);
		DirectX::XMVECTOR pos0 = XMLoadFloat3(&boneAnimation.positions[p0Index].position);
		DirectX::XMVECTOR pos1 = XMLoadFloat3(&boneAnimation.positions[p1Index].position);
		position = DirectX::XMVectorLerp(pos0, pos1, scaleFactor);
	}

	//Interpolate rotation
	DirectX::XMVECTOR rotation;
	if (boneAnimation.rotations.size() == 1)
	{
		rotation = XMLoadFloat4(&boneAnimation.rotations[0].orientation);
	}
	else
	{
		int r0Index = FindRotationIndex(boneAnimation.rotations, animationTime);
		int r1Index = r0Index + 1;
		float scaleFactor = GetScaleFactor(boneAnimation.rotations[r0Index].timeStamp,
			boneAnimation.rotations[r1Index].timeStamp,
			animationTime);
		DirectX::XMVECTOR rot0 = XMLoadFloat4(&boneAnimation.rotations[r0Index].orientation);
		DirectX::XMVECTOR rot1 = XMLoadFloat4(&boneAnimation.rotations[r1Index].orientation);
		rotation = DirectX::XMQuaternionSlerp(rot0, rot1, scaleFactor);
	}

	//Interpolate scale
	DirectX::XMVECTOR scale;
	if (boneAnimation.scales.size() == 1)
	{
		scale = XMLoadFloat3(&boneAnimation.scales[0].scale);
	}
	else
	{
		int s0Index = FindScaleIndex(boneAnimation.scales, animationTime);
		int s1Index = s0Index + 1;
		float scaleFactor = GetScaleFactor(boneAnimation.scales[s0Index].timeStamp,
			boneAnimation.scales[s1Index].timeStamp,
			animationTime);
		DirectX::XMVECTOR scale0 = XMLoadFloat3(&boneAnimation.scales[s0Index].scale);
		DirectX::XMVECTOR scale1 = XMLoadFloat3(&boneAnimation.scales[s1Index].scale);
		scale = DirectX::XMVectorLerp(scale0, scale1, scaleFactor);
	}

	//Combine transformations
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(position);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(scale);

	return scaleMatrix * rotationMatrix * translationMatrix;
}

void AnimationManager::AnimationUpdate(Animator3DComponent& animator, Model& model)
{
	if (!animator.currentAnimation) return;

	animator.currentTime += animator.currentAnimation->ticksPerSecond * Time::deltaTime;
	animator.currentTime = fmod(animator.currentTime, animator.currentAnimation->duration);

	if (animator.isBlending && animator.nextAnimation)
	{
		//Update next animation time
		animator.nextTime += animator.nextAnimation->ticksPerSecond * Time::deltaTime;
		animator.nextTime = fmod(animator.nextTime, animator.nextAnimation->duration);

		//Update blend timer
		animator.blendTime += Time::deltaTime;

		//Calculate blend factor (0 = current anim, 1 = next anim)
		float blendFactor = animator.blendTime / animator.blendDuration;

		if (blendFactor >= 1.0f)
		{
			//Blending complete, switch to next animation
			animator.currentAnimation = animator.nextAnimation;
			animator.currentTime = animator.nextTime;
			animator.nextAnimation = nullptr;
			animator.isBlending = false;
		}
		else
		{
			//Blend between animations
			CalculateBlendedBoneTransforms(animator.rootNodeIndex, DirectX::XMMatrixIdentity(), blendFactor, animator, model);
			return;
		}
	}

	CalculateBoneTransforms(animator.rootNodeIndex, DirectX::XMMatrixIdentity(), animator, model);
}

void AnimationManager::AnimationSwitch(StateComponent state, Animator3DComponent& animator, Model& model)
{
	if (state.currentState == state.previousState)
		return;

	switch (state.currentState)
	{
	case ActionState::Idle:
		BlendToAnimation(AnimationState::Idle, model, animator);
		break;
	case ActionState::Walking:
		BlendToAnimation(AnimationState::Walking, model, animator);
		break;
	case ActionState::Running:
		BlendToAnimation(AnimationState::Running, model, animator);
		break;
	case ActionState::Attacking:
		BlendToAnimation(AnimationState::Attacking, model, animator);
		break;
	case ActionState::Dead:
		break;
	case ActionState::Count:
		break;
	default:
		break;
	}
}

void AnimationManager::Update()
{
	for (auto e : registry.View<Animator3DComponent, ModelComponent, StateComponent>())
	{
		auto& animator = registry.GetComponent<Animator3DComponent>(e);
		auto& modelComponent = registry.GetComponent<ModelComponent>(e);
		auto& state = registry.GetComponent<StateComponent>(e);

		Model& model = modelManager.GetModel(modelComponent.modelId).m_model;

		AnimationSwitch(state, animator, model);
		AnimationUpdate(animator, model);
	}
}

AnimationManager::AnimationManager(Registry& registry, ModelManager& modelManager) : registry(registry), modelManager(modelManager) {}