#pragma once
#include <GameState/GameState.h>
#include <StateRegistry/StateID.h>


class LoadingState : public Umi::GameState
{
private:
	Umi::EngineContext& engineContext;
	float progress = 0.0f;
	Umi::StringID nextState;
	Umi::Entity loadBar = Umi::INVALID_ENTITY;

public:
	Umi::Signal HandleInput() override;
	Umi::Signal Update() override;

	LoadingState(Umi::EngineContext& engineContext, Umi::StringID stateId, Umi::PrefabRequest request);
	~LoadingState() override = default;
};