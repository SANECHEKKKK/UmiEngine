#pragma once
#include <StringID/StringID.h>

namespace Umi
{
	namespace AnimationState
	{
		inline constexpr StringID Idle = "Idle"_sid;
		inline constexpr StringID Walking = "Walking"_sid;
		inline constexpr StringID Running = "Running"_sid;
		inline constexpr StringID Jump = "Jump"_sid;
		inline constexpr StringID Attacking = "Attacking"_sid;
	}
}