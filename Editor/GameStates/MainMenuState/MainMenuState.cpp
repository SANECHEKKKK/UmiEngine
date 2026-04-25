#include <MainMenuState/MainMenuState.h>
#include <Texture/TextureComponent.h>
#include <Texture/TextureManager.h>
#include <Input/InputManager.h>
#include <Transform/TransformComponent.h>
#include <Prefab/PrefabManager.h>
#include <Settings/Settings.h>
using namespace Umi;

Signal MainMenuState::HandleInput()
{
	buttonSystem.Update(stateID);

	if (engineContext.inputManager.keyboard.IsKeyTrigger(Keyboard_Keys::KK_ESCAPE))
	{
		return Signal(SignalType::CloseGame);
	}
	return Signal();
}

Signal MainMenuState::Update()
{
	return Signal();
}


MainMenuState::MainMenuState(EngineContext& engineContext)
	: GameState(engineContext)
{
	stateID = MainMenuStateID;

	mainCamera.SetPosition(PLAY_STATE_CAMERA_POSITION);
}