#include <LoadingState/LoadingState.h>
#include <Texture/TextureComponent.h>
#include <Texture/TextureManager.h>
#include <Transform/TransformComponent.h>
#include <LoadManager/LoadManager.h>
#include <Prefab/PrefabManager.h>
#include <Bar/BarComponent.h>

using namespace Umi;

Signal LoadingState::HandleInput()
{
	return Signal();
}

Signal LoadingState::Update()
{
	progress = engineContext.loadManager.GetProgress();
	engineContext.registry.GetComponent<BarComponent>(loadBar).currentValue = engineContext.loadManager.GetCompletedSteps();

	if (engineContext.loadManager.Update().type == SignalType::FinishedLoading)
		return Signal(SignalType::FinishedLoading, nextState);

	return Signal();
}

LoadingState::LoadingState(EngineContext& engineContext, StringID stateId, PrefabRequest request)
	: GameState(engineContext), engineContext(engineContext), nextState(stateId), progress(0.0f)
{
	engineContext.loadManager.LoadThread(request);

	CreateUi("LoadBackground");
	loadBar = CreateUi("LoadBar");

	engineContext.registry.GetComponent<BarComponent>(loadBar).maxValue = engineContext.loadManager.GetTotalSteps();
}
