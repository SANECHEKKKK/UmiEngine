module;
#include <cmath>
module InputSystem;

using namespace Umi;

void InputSystem::Update()
{
	inputVelocity = Vector3();
	
	if (Keyboard::IsKeyDown(KK_W))
	{
		inputVelocity.y += 1.0f;
	}
	if (Keyboard::IsKeyDown(KK_S))
	{
		inputVelocity.y -= 1.0f;
	}
	if (Keyboard::IsKeyDown(KK_A))
	{
		inputVelocity.x -= 1.0f;
	}
	if (Keyboard::IsKeyDown(KK_D))
	{
		inputVelocity.x += 1.0f;
	}
	
	inputVelocity.normalize();

	inputRotation = atan2f(-inputVelocity.x, -inputVelocity.y);
}
