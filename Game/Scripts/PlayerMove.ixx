module;
#include "cmath"
#include <DirectXMath.h>
export module Scripts.PlayerMove;

import Engine;
import Scripts.EnemyScript;

export namespace Umi
{
	class PlayerMove : public BasicScript
	{
	private:

		Vector3 velocity{ 0.0f, 0.0f, 0.0f };
		const float movementSpeed = 20.0f;

		enum class PlayerState
		{
			Idle,
			Attacking,
			Moving,
			NONE
		};

		PlayerState state = PlayerState::NONE;
		PlayerState previousState = PlayerState::NONE;

	public:
		void Start() override
		{

		}

		void Update() override
		{

			Transform& transform = GetComponent<Transform>();

			previousState = state;
			state = PlayerState::Idle;


			if (InputSystem::inputVelocity.x != 0.0f || InputSystem::inputVelocity.y != 0.0f)
			{
				if (InputSystem::inputRotation != DirectX::XMConvertToRadians(transform.rot.y))
				{
					float inputRotationDegrees = DirectX::XMConvertToDegrees(InputSystem::inputRotation);

					//transform.rot.y = std::fmod(transform.rot.y, 180.0f);

					if (transform.rot.y > 180.0f)
					{
						transform.rot.y -= 360.0f;
					}
					else if (transform.rot.y < -180.0f)
					{
						transform.rot.y += 360.0f;
					}

					float rotationDiff = inputRotationDegrees - transform.rot.y;
					if (rotationDiff > 180.0f)
						rotationDiff -= 360.0f;
					else if (rotationDiff < -180.0f)
						rotationDiff += 360.0f;
					float rotationStep = 400 * Time::deltaTime;
					if (abs(rotationDiff) <= rotationStep)
						transform.rot.y = inputRotationDegrees;
					else
						transform.rot.y += (rotationDiff > 0.0f ? rotationStep : -rotationStep);

				}
				velocity.x += InputSystem::inputVelocity.x * movementSpeed * Time::deltaTime;
				velocity.z += InputSystem::inputVelocity.y * movementSpeed * Time::deltaTime;
				InputSystem::inputVelocity = { 0.0f, 0.0f, 0.0f };

				state = PlayerState::Moving;
			}

			if (velocity.x != 0.0f || velocity.z != 0.0f)
			{
				velocity.x -= velocity.x * 10.0f * Time::deltaTime;
				velocity.z -= velocity.z * 10.0f * Time::deltaTime;
				if (abs(velocity.x) < 0.01f) velocity.x = 0.0f;
				if (abs(velocity.z) < 0.01f) velocity.z = 0.0f;
				if (velocity.length() > movementSpeed) velocity = velocity.normalized() * movementSpeed;
				transform.pos.x += velocity.x * Time::deltaTime;
				transform.pos.z += velocity.z * Time::deltaTime;
			}



			//-------------ATTACK-------------
			if (Keyboard::IsKeyDown(KK_SPACE))
			{
				state = PlayerState::Attacking;
				for (const auto e : CheckOverlap(transform.pos + -transform.Forward() * 1.5f, { 1.0f, 1.0f, 1.0f }))
				{
					if (HasTag(e, "Enemy"))
					{
						if (auto script = GetScript<EnemyScript>(e))
						{
							script->GiveDamage(1);
						}
					}
				}
			}
			//--------------------------------

			UpdateState();

			//if (Keyboard::IsKeyTrigger(KK_P))
			//{
			//	CloseGame();
			//}
		}


		void UpdateState()
		{
			auto& anim = GetComponent<Animator>();
			
			if (previousState == state)
				return;

			switch (state)
			{
			case PlayerState::Idle:
				anim.Play(AnimationState::Idle);
				break;

			case PlayerState::Attacking:
				anim.Play(AnimationState::Attacking);
				break;

			case PlayerState::Moving:
				anim.Play(AnimationState::Running);
				break;

			case PlayerState::NONE:
				break;

			default:
				break;
			}
		}
	};
}
