module;
#include <cstdint>
export module Entity;

export namespace Umi
{
	using Entity = uint32_t;

	constexpr Entity INVALID_ENTITY = static_cast<Entity>(-1);
}