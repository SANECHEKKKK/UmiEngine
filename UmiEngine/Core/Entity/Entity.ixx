module;
#include <cstdint>
#include <limits>
export module Entity;

export namespace Umi
{
	enum class Entity : uint32_t {};

	inline constexpr Entity INVALID_ENTITY = static_cast<Entity>(std::numeric_limits<uint32_t>::max());

	//       ++e
	Entity operator++(Entity& e)
	{
		e = static_cast<Entity>(static_cast<uint32_t>(e) + 1); // Increment the underlying uint32_t value and cast back to Entity
		return e;
	}

	//	     e++
	Entity operator++(Entity& e, int) // int to indicate post-increment
	{
		Entity old = e;
		++e;
		return old;
	}
}