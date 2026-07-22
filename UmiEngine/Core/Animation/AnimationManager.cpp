module;
#include <DirectXMath.h>

#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
module AnimationManager;

import Registry;
import Model;
import ModelManager;
import Animation;
import Animator;

using namespace Umi;
using namespace DirectX;

template <class T>
static int FindKey(const std::vector<T>& keys, float t) noexcept
{
	int lo = 0;
	int hi = static_cast<int>(keys.size()) - 1;
	while (lo + 1 < hi)
	{
		const int mid = (lo + hi) / 2;
		if (keys[mid].time <= t) lo = mid; else hi = mid;
	}
	return lo;
}

static float KeyFactor(float a, float b, float t) noexcept
{
	const float span = b - a;
	if (span <= 0.0f) return 0.0f;
	return std::clamp((t - a) / span, 0.0f, 1.0f);
}

static XMVECTOR SampleVec3(const std::vector<KeyVec3>& keys, float t) noexcept
{
	if (keys.empty())  return XMVectorZero();
	if (keys.size() == 1) return XMLoadFloat3(&keys[0].value);

	const int i = FindKey(keys, t);
	const float f = KeyFactor(keys[i].time, keys[i + 1].time, t);
	return XMVectorLerp(XMLoadFloat3(&keys[i].value),
	                    XMLoadFloat3(&keys[i + 1].value), f);
}

static XMVECTOR SampleQuat(const std::vector<KeyQuat>& keys, float t) noexcept
{
	if (keys.empty())  return XMQuaternionIdentity();
	if (keys.size() == 1) return XMLoadFloat4(&keys[0].value);

	const int i = FindKey(keys, t);
	const float f = KeyFactor(keys[i].time, keys[i + 1].time, t);
	return XMQuaternionSlerp(XMLoadFloat4(&keys[i].value),
	                         XMLoadFloat4(&keys[i + 1].value), f);
}

static XMVECTOR BlendQuat(XMVECTOR a, XMVECTOR b, float w) noexcept
{
	if (XMVectorGetX(XMVector4Dot(a, b)) < 0.0f)
		b = XMVectorNegate(b);
	return XMQuaternionNormalize(XMQuaternionSlerp(a, b, w));
}

// ---------------------------------------------------------------------------

void AnimationManager::ResolveSlots(Animator& animator, const Skeleton&,
                                    const std::vector<AnimationClip>& clips)
{
	for (auto& slot : animator.slots)
	{
		if (slot.clipIndex >= 0) continue;

		slot.keyHash = AnimHash(slot.key);
		for (int i = 0; i < static_cast<int>(clips.size()); ++i)
		{
			if (clips[i].name == slot.clipName) { slot.clipIndex = i; break; }
		}
	}
}

void AnimationManager::ApplyPending(Animator& animator)
{
	if (!animator.hasPendingPlay) return;
	animator.hasPendingPlay = false;

	const int slotIndex = animator.FindSlot(animator.pendingPlay);
	if (slotIndex < 0) return;

	const AnimationSlot& slot = animator.slots[slotIndex];
	if (slot.clipIndex < 0) return;

	const float fade = animator.pendingFade >= 0.0f
		? animator.pendingFade
		: animator.blendDuration;

	const int target = animator.blending ? animator.nextClip : animator.currentClip;
	if (target == slot.clipIndex) return;

	animator.finished = false;

	if (animator.currentClip < 0 || fade <= 0.0f)
	{
		animator.currentClip  = slot.clipIndex;
		animator.currentTime  = 0.0f;
		animator.currentLoops = slot.loop;
		animator.blending     = false;
		animator.nextClip     = -1;
		return;
	}

	animator.nextClip      = slot.clipIndex;
	animator.nextTime      = 0.0f;
	animator.nextLoops     = slot.loop;
	animator.blendTime     = 0.0f;
	animator.blendDuration = fade;
	animator.blending      = true;
}

static void AdvanceTrack(float& time, const AnimationClip& clip,
                         bool loop, float dt, bool& finished)
{
	const float length = clip.DurationSeconds();
	if (length <= 0.0f) { time = 0.0f; return; }

	time += dt;
	if (time >= length)
	{
		if (loop) time = std::fmod(time, length);
		else { time = length; finished = true; }
	}
}

void AnimationManager::AdvanceTime(Animator& animator,
                                   const std::vector<AnimationClip>& clips,
                                   float deltaTime)
{
	const float dt = deltaTime * animator.speed;

	AdvanceTrack(animator.currentTime, clips[animator.currentClip],
	             animator.currentLoops, dt, animator.finished);

	if (!animator.blending) return;

	bool ignored = false;
	AdvanceTrack(animator.nextTime, clips[animator.nextClip],
	             animator.nextLoops, dt, ignored);

	animator.blendTime += deltaTime;
	if (animator.blendDuration <= 0.0f ||
	    animator.blendTime >= animator.blendDuration)
	{
		animator.currentClip  = animator.nextClip;
		animator.currentTime  = animator.nextTime;
		animator.currentLoops = animator.nextLoops;
		animator.nextClip     = -1;
		animator.blending     = false;
		animator.finished     = false;
	}
}

void AnimationManager::SampleClip(const AnimationClip& clip,
                                  const Skeleton& skeleton,
                                  float timeSeconds, bool blendInto, float weight)
{
	const float ticks = timeSeconds * clip.ticksPerSecond;
	const size_t nodeCount = skeleton.nodes.size();

	for (size_t i = 0; i < nodeCount; ++i)
	{
		XMVECTOR t, r, s;

		const int channelIndex =
			i < clip.nodeToChannel.size() ? clip.nodeToChannel[i] : -1;

		if (channelIndex >= 0)
		{
			const NodeChannel& ch = clip.channels[channelIndex];
			t = SampleVec3(ch.positions, ticks);
			r = SampleQuat(ch.rotations, ticks);
			s = ch.scales.empty() ? XMVectorSplatOne() : SampleVec3(ch.scales, ticks);
		}
		else
		{
			XMMATRIX bind = XMLoadFloat4x4(&skeleton.nodes[i].localTransform);
			if (!XMMatrixDecompose(&s, &r, &t, bind))
			{
				s = XMVectorSplatOne();
				r = XMQuaternionIdentity();
				t = XMVectorZero();
			}
		}

		if (!blendInto)
		{
			localT[i] = t; localR[i] = r; localS[i] = s;
		}
		else
		{
			localT[i] = XMVectorLerp(localT[i], t, weight);
			localS[i] = XMVectorLerp(localS[i], s, weight);
			localR[i] = BlendQuat(localR[i], r, weight);
		}
	}
}

void AnimationManager::BuildPalette(Animator& animator, const Skeleton& skeleton)
{
	const size_t nodeCount = skeleton.nodes.size();

	for (size_t i = 0; i < nodeCount; ++i)
	{
		const XMMATRIX local =
			XMMatrixScalingFromVector(localS[i]) *
			XMMatrixRotationQuaternion(localR[i]) *
			XMMatrixTranslationFromVector(localT[i]);

		const int parent = skeleton.nodes[i].parentIndex;
		globals[i] = (parent >= 0) ? local * globals[parent] : local;
	}

	const XMMATRIX globalInverse = XMLoadFloat4x4(&skeleton.globalInverseTransform);
	const int boneCount = std::min<int>(static_cast<int>(skeleton.bones.size()), MAX_BONES);

	for (int b = 0; b < boneCount; ++b)
	{
		const BoneInfo& bone = skeleton.bones[b];
		const XMMATRIX offset = XMLoadFloat4x4(&bone.offsetMatrix);
		const XMMATRIX final  = offset * globals[bone.nodeIndex] * globalInverse;

		XMStoreFloat4x4(&animator.palette[b], final);
	}

	for (int b = boneCount; b < MAX_BONES; ++b)
		XMStoreFloat4x4(&animator.palette[b], XMMatrixIdentity());
}

void AnimationManager::Update(float deltaTime)
{
	for (auto e : registry.View<Model, Animator>())
	{
		auto& model    = registry.GetComponent<Model>(e);
		auto& animator = registry.GetComponent<Animator>(e);

		if (model.id == INVALID_MODELID) continue;

		auto& modelData = modelManager.GetModelData(model.id);
		const Skeleton& skeleton = modelData.skeleton;
		const auto& clips = modelData.animations;

		if (!skeleton.HasSkin() || clips.empty()) continue;

		ResolveSlots(animator, skeleton, clips);
		ApplyPending(animator);

		if (animator.currentClip < 0) continue;

		AdvanceTime(animator, clips, deltaTime);

		const size_t nodeCount = skeleton.nodes.size();
		if (localT.size() < nodeCount)
		{
			localT.resize(nodeCount); localR.resize(nodeCount);
			localS.resize(nodeCount); globals.resize(nodeCount);
		}

		SampleClip(clips[animator.currentClip], skeleton,
		           animator.currentTime, false, 0.0f);

		if (animator.blending && animator.blendDuration > 0.0f)
		{
			const float w = std::clamp(
				animator.blendTime / animator.blendDuration, 0.0f, 1.0f);
			SampleClip(clips[animator.nextClip], skeleton,
			           animator.nextTime, true, w);
		}

		BuildPalette(animator, skeleton);
	}
}

AnimationManager::AnimationManager(Registry& registry, ModelManager& modelManager)
	: registry(registry), modelManager(modelManager)
{}