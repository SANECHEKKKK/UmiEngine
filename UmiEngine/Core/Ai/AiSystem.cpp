#include <Ai/AiSystem.h>
#include <Registry/Registry.h>
#include <Ai/AiComponent.h>
#include <Transform/TransformComponent.h>
#include <Input/InputComponent.h>
#include <Tag/TagComponent.h>
#include <Time/Time.h>
#include <State/StateComponent.h>

using namespace Umi;

inline void AiSystem::UpdateAiFollowBehavior(AiFollowBehaviorComponent& ai, InputComponent& input, Transform3DComponent& transform, StateComponent& state)
{
	if (playerEntity == INVALID_ENTITY) [[unlikely]]
		return;

	if (!registry.HasComponent<Transform3DComponent>(playerEntity)) [[unlikely]]
		return;

	auto& playerTransform = registry.GetComponent<Transform3DComponent>(playerEntity);

	input.inputVelocity = playerTransform.pos - transform.pos;
	input.inputRotation = atan2f(-input.inputVelocity.x, -input.inputVelocity.z);

	if (transform.rot.y != input.inputRotation)
	{
		transform.rot.y = std::fmod(transform.rot.y, TWO_PI);

		if (transform.rot.y > PI)
		{
			transform.rot.y -= TWO_PI;
		}
		else if (transform.rot.y < -PI)
		{
			transform.rot.y += TWO_PI;
		}

		float rotationDiff = input.inputRotation - transform.rot.y;
		if (rotationDiff > PI)
			rotationDiff -= TWO_PI;
		else if (rotationDiff < -PI)
			rotationDiff += TWO_PI;
		float rotationStep = 10 * Time::deltaTime;
		if (abs(rotationDiff) <= rotationStep)
			transform.rot.y = input.inputRotation;
		else
			transform.rot.y += (rotationDiff > 0.0f ? rotationStep : -rotationStep);
	}
	if (input.inputVelocity.x != 0.0f || input.inputVelocity.z != 0.0f)
	{
		input.inputVelocity.normalize();
		state.currentState = ActionState::Walking;
	}
	else
		state.currentState = ActionState::Idle;

}

void AiSystem::Update()
{
	for (auto e : registry.View<AiFollowBehaviorComponent, InputComponent, Transform3DComponent, StateComponent>())
	{
		auto& ai = registry.GetComponent<AiFollowBehaviorComponent>(e);
		auto& input = registry.GetComponent<InputComponent>(e);
		auto& transform = registry.GetComponent<Transform3DComponent>(e);
		auto& state = registry.GetComponent<StateComponent>(e);

		UpdateAiFollowBehavior(ai, input, transform, state);
	}
}

AiSystem::AiSystem(Registry& registry, Entity& entity) : registry(registry), playerEntity(entity) {}
