#pragma once
#include <GameState/GameState.h>
#include <Button/ButtonSystem.h>

class MainMenuState : public Umi::GameState
{
private:
	Umi::ButtonSystem buttonSystem{ engineContext.registry, engineContext.inputManager };
	const Umi::Vector3 PLAY_STATE_CAMERA_POSITION = { 0.0f, 12.0f, -7.0f };

public:
	Umi::Signal HandleInput() override;
	Umi::Signal Update() override;

	MainMenuState(Umi::EngineContext& engineContext);
	~MainMenuState() override = default;
};