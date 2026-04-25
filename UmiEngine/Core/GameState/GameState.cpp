#include <Window/Window.h>
#include <GameState/GameState.h>
#include <EngineContext/EngineContext.h>
#include <Entity/Entity.h>
#include <Prefab/PrefabManager.h>
#include <Settings/Settings.h>
#include <Error/ErrorManager.h>
#include <stdexcept>

using namespace Umi;

Entity GameState::CreateFromPrefab(const PrefabName& prefabName)
{
	if (!engineContext.prefabManager.CheckPrefabExists(prefabName)) [[unlikely]]
	{
		//engineContext.errorManager.CreateError(ErrorCode::FailedToLoadPrefab, true);
		//throw std::runtime_error("Failed to load prefab " + prefabName + "\n");
		return INVALID_ENTITY;
	}

	Entity e = engineContext.registry.CreateEntity();
	engineContext.prefabManager.ApplyPrefab(engineContext.registry, prefabName, e);
	engineContext.registry.AddComponent<StateIDComponent>(e, StateIDComponent{ stateID });
	return e;
}

Entity GameState::CreateUi(const PrefabName& prefabName)
{
	Entity e = engineContext.registry.CreateEntity();
	engineContext.prefabManager.ApplyPrefab(engineContext.registry, prefabName, e);
	engineContext.registry.AddComponent<StateIDComponent>(e, StateIDComponent{ stateID });
	engineContext.uiSystem.RecalculateAll(engineContext.settings.getResolution());

	return e;
}

void GameState::Delete(Entity e)
{
	if (e != INVALID_ENTITY)
		engineContext.registry.DestroyEntity(e);
}

GameState::GameState(EngineContext& engineContext) : engineContext(engineContext) {}

GameState::~GameState()
{
	for (auto e : engineContext.registry.View<StateIDComponent>())
	{
		auto& state = engineContext.registry.GetComponent<StateIDComponent>(e);
		if (state.stateID == stateID)
		{
			engineContext.registry.DestroyEntity(e);
		}
	}
}