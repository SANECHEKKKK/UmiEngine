module;
#include <cstdint>
#include <limits>
export module Texture;

export namespace Umi
{
	enum class TextureID : uint32_t {};

	inline constexpr TextureID INVALID_TEXTUREID = static_cast<TextureID>(std::numeric_limits<uint32_t>::max());

	struct Texture 
	{
		TextureID id = INVALID_TEXTUREID;
	};
}