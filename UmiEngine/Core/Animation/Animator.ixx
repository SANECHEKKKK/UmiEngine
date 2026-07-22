module;
#include <DirectXMath.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <EngineApi/EngineApi.h>
export module Animator;

import Animation;

export namespace Umi
{
	using AnimKey = uint32_t;

	constexpr AnimKey AnimHash(std::string_view s) noexcept
	{
		AnimKey h = 2166136261u;
		for (char c : s) { h ^= static_cast<AnimKey>(c); h *= 16777619u; }
		return h;
	}

	constexpr AnimKey operator""_anim(const char* s, size_t n) noexcept
	{
		return AnimHash(std::string_view(s, n));
	}

	namespace AnimationState
	{
		inline constexpr AnimKey Idle = "Idle"_anim;
		inline constexpr AnimKey Walking = "Walking"_anim;
		inline constexpr AnimKey Running = "Running"_anim;
		inline constexpr AnimKey Jump = "Jump"_anim;
		inline constexpr AnimKey Attacking = "Attacking"_anim;
	}

	struct ENGINE_API AnimationSlot
	{
		std::string key;
		std::string clipName;
		AnimKey     keyHash = 0;
		int         clipIndex = -1;
		bool        loop = true;
		float       speed = 1.0f;
	};

	struct ENGINE_API Animator
	{
		std::vector<AnimationSlot> slots;

		int   currentClip = -1;
		int   nextClip = -1;
		float currentTime = 0.0f;
		float nextTime = 0.0f;

		bool  blending = false;
		float blendTime = 0.0f;
		float blendDuration = 0.15f;

		float speed = 1.0f;
		bool  currentLoops = true;
		bool  nextLoops = true;
		bool  finished = false;

		std::vector<DirectX::XMFLOAT4X4> palette;

		AnimKey pendingPlay = 0;
		float   pendingFade = -1.0f;
		bool    hasPendingPlay = false;

		Animator() { palette.resize(MAX_BONES); }

		void Play(AnimKey state, float fade = -1.0f) noexcept
		{
			pendingPlay = state;
			pendingFade = fade;
			hasPendingPlay = true;
		}

		void SetSpeed(float s) noexcept { speed = s; }
		bool IsFinished() const noexcept { return finished; }

		int FindSlot(AnimKey state) const noexcept
		{
			for (int i = 0; i < static_cast<int>(slots.size()); ++i)
				if (slots[i].keyHash == state) return i;
			return -1;
		}
	};
}