export module CameraMove;

import Engine;

using namespace Umi;

export class CameraMove : public BasicScript
{
private:

public:
	void Start() override
	{

	};

	void Update() override
	{
		auto& transform = GetComponent<Transform>(this);

		if (Keyboard::IsKeyDown(KK_W))
		{
			transform.pos.z += 0.1f;
		}
		if (Keyboard::IsKeyDown(KK_S))
		{
			transform.pos.z -= 0.1f;
		}
		if (Keyboard::IsKeyDown(KK_A))
		{
			transform.pos.x -= 0.1f;
		}
		if (Keyboard::IsKeyDown(KK_D))
		{
			transform.pos.x += 0.1f;
		}
		if (Keyboard::IsKeyDown(KK_Q))
		{
			transform.rot.z -= 0.1f;
		}
		if (Keyboard::IsKeyDown(KK_E))
		{
			transform.rot.z += 0.1f;
		}
	};

};