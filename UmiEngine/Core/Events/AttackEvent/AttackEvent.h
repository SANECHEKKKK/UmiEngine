#pragma once
#include <Entity/Entity.h>

namespace Umi
{
	struct AttackEvent
	{
		Entity attacker;
		Entity target;

		AttackEvent(Entity attacker, Entity target) : attacker(attacker), target(target) {}
	};
}