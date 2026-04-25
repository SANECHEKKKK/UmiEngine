#pragma once
#include <Registry/Registry.h>
#include <Entity/Entity.h>
#include <Signal/Signal.h>
#include <Tag/TagComponent.h>
#include <StringID/StringID.h>

namespace Umi
{
	class Registry;
	class InputManager;

	class ButtonSystem
	{
	private:
		Registry& registry;

		InputManager& inputManager;

		Signal HandleButtonClick(Entity e);
		
		template<typename T>
		void CheckCreateButtonEvent(Entity e)
		{
			if (registry.HasComponent<T>(e))
			{
				auto newEntity = registry.CreateEntity();
				registry.AddComponent<T>(newEntity, registry.GetComponent<T>(e));
				registry.AddComponent<EventTag>(newEntity, EventTag());
			}
		}

	public:
		Signal Update(StringID stateID);

		ButtonSystem(Registry& registry, InputManager& inputManager) : registry(registry), inputManager(inputManager) {}
		~ButtonSystem() = default;
	};
}