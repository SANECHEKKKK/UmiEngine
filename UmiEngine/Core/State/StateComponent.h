#pragma once

namespace Umi
{
	enum class ActionState
	{
		Idle,
		Walking,
		Running,
		Jumping,
		Attacking,
		Dead,

		Count
	};

	struct StateComponent
	{
		ActionState currentState = ActionState::Count;
		ActionState previousState = ActionState::Count;
	};
}