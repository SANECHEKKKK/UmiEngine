#pragma once
#include <functional>
#include <Prefab/PrefabRequest.h>
#include <GameState/GameState.h>

namespace Umi
{
	struct StateDescription
	{
		PrefabRequest prefabRequest;
		std::function<std::unique_ptr<GameState>(EngineContext&)> create;
	};
}