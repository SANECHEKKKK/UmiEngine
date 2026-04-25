#pragma once


namespace Umi
{
	class InputManager;
	class Registry;
	struct InputComponent;

	class InputSystem
	{
	private:
		Registry& registry;
		InputManager& inputManager;

		inline void HandleInput(InputComponent& input, struct StateComponent& state);

	public:
		void Update();

		InputSystem(InputManager& inputManager, Registry& registry);
	};
}