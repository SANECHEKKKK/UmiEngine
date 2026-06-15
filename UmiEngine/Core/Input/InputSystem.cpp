//#include <Input/InputSystem.h>
//#include <Input/InputManager.h>
//#include <Input/InputComponent.h>
//#include <Registry/Registry.h>
//#include <State/StateComponent.h>
//#include <Tag/TagComponent.h>
//
//using namespace Umi;
//
//InputSystem::InputSystem(InputManager& inputManager, Registry& registry) : inputManager(inputManager), registry(registry) {}
//
//void InputSystem::Update()
//{
//	for (auto e : registry.View<InputComponent, StateComponent, PlayerTag>())
//	{
//		auto& input = registry.GetComponent<InputComponent>(e);
//		auto& state = registry.GetComponent<StateComponent>(e);
//
//		HandleInput(input, state);
//	}
//}
//
//void InputSystem::HandleInput(InputComponent& input, StateComponent& state)
//{
//	state.previousState = state.currentState;
//
//	if (inputManager.keyboard.IsKeyDown(KK_A))
//	{
//		input.inputVelocity.x += -1.0f;
//	}
//	if (inputManager.keyboard.IsKeyDown(KK_D))
//	{
//		input.inputVelocity.x += 1.0f;
//	}
//	if (inputManager.keyboard.IsKeyDown(KK_W))
//	{
//		input.inputVelocity.z += 1.0f;
//	}
//	if (inputManager.keyboard.IsKeyDown(KK_S))
//	{
//		input.inputVelocity.z += -1.0f;
//	}
//	if (input.inputVelocity.x != 0.0f || input.inputVelocity.z != 0.0f)
//	{
//		input.inputVelocity.normalize();
//		state.currentState = ActionState::Running;
//	}
//	else
//		state.currentState = ActionState::Idle;
//
//
//	//attack input
//	if (inputManager.keyboard.IsKeyDown(KK_LEFT))
//	{
//		input.attackInputVelocity.x -= 1.0f;
//	}
//	if (inputManager.keyboard.IsKeyDown(KK_RIGHT))
//	{
//		input.attackInputVelocity.x += 1.0f;
//	}
//	if (inputManager.keyboard.IsKeyDown(KK_UP))
//	{
//		input.attackInputVelocity.z += 1.0f;
//	}
//	if (inputManager.keyboard.IsKeyDown(KK_DOWN))
//	{
//		input.attackInputVelocity.z -= 1.0f;
//	}
//	if (input.attackInputVelocity.x != 0.0f || input.attackInputVelocity.z != 0.0f)
//	{
//		input.attackInputVelocity.normalize();
//		state.currentState = ActionState::Attacking;
//	}
//}