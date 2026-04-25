#pragma once
#include <cstdint>

namespace Umi
{
	using StringID = uint32_t;

	inline constexpr StringID StringIDNone = 0;

	inline constexpr StringID GenerateStringID(const char* str)
	{
		StringID hash = 2166136261u;
		while (*str)
		{
			hash ^= static_cast<uint8_t>(*str++);
			hash *= 16777619u;
		}
		return hash;
	}

	constexpr StringID operator"" _sid(const char* str, size_t)
	{
		return GenerateStringID(str);
	}
}