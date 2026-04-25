#pragma once
#include <Entity/Entity.h>

namespace Umi
{
	class Registry;

	class AiSystem
	{
	private:
		Registry& registry;

		Entity& playerEntity;

		inline void UpdateAiFollowBehavior(struct AiFollowBehaviorComponent& ai, struct InputComponent& input, struct Transform3DComponent& transform, struct StateComponent& state);

	public:
		void Update();

		AiSystem(Registry& registry, Entity& entity);
	};
}