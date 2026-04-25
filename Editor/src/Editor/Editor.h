#pragma once
#include <memory>
#include <vector>
#include <functional>

#include <Window/Window.h>

//----------ENGINE CONTEXT----------//
#include <EngineContext/EngineContext.h>
#include <Settings/Settings.h>
#include <Rendering/Renderer.h>
#include <Texture/TextureManager.h>
#include <Prefab/PrefabManager.h>
#include <LoadManager/LoadManager.h>
#include <Input/InputManager.h>
#include <Model/ModelManager.h>
#include <Error/ErrorManager.h>
//----------------------------------//

#include <EditorContext/EditorContext.h>

#include <Prefab/PrefabRequest.h>
#include <StateRegistry/StateRegistry.h>
#include <Registry/Registry.h>

#include <Rendering/RenderSystem.h>
#include <Ui/UiSystem.h>
#include <ImGui/ImGuiManager.h>

class Editor
{
private:
private:
	bool isRunning = true;

	Umi::Settings settings;
	Umi::TextureManager textureManager;
	Umi::InputManager inputManager{ settings };
	Umi::EngineContext engineContext{ settings, textureManager, renderer, prefabManager, loadManager, inputManager, registry, uiSystem, modelManager, errorManager };
	Umi::Window window;
	Umi::Renderer renderer;
	Umi::ImGuiManager imGuiManager;
	Umi::EditorContext editorContext;
	Umi::PrefabManager prefabManager{ engineContext };
	Umi::LoadManager loadManager{ engineContext };
	Umi::ModelManager modelManager;
	Umi::ErrorManager errorManager{ registry };

	Umi::Registry registry;

	Umi::RenderSystem renderSystem{ registry, renderer };
	Umi::UiSystem uiSystem{ registry };


	std::vector<std::unique_ptr<Umi::GameState>> gameStack;

	Umi::PrefabRequest bootPrefabs{
		.folders = { "LoadingState", "SettingsState" }
	};

	bool bootPrefabsLoaded = false;

	Umi::StateRegistry stateRegistry;

	//----------------DEBUG RELATED----------------//
#ifdef _DEBUG
	int g_CountFps;
	char g_DebugStr[2048] = "DEBUG WINDOW";

	int debugFps = 0;
	float debugTime = 0.0f;
	LARGE_INTEGER debugTimeLast;

	void UpdateDebugTitle();
#endif // !_DEBUG
	//---------------------------------------------//

	LARGE_INTEGER currentTime;
	LARGE_INTEGER lastTime;
	LARGE_INTEGER frequency;

	float elapsed;

	void FrameTick();

	//---------------------------//
	Umi::Signal HandleInput();
	Umi::Signal Update();
	void Render();
	Umi::Signal HandleEvents();
	//---------------------------//

	//----------------STACK RELATED----------------//
	void AddState(std::unique_ptr<Umi::GameState>);
	void AddState(Umi::StringID stateId);
	void PopState();
	void ClearStack();
	void CloseGame();

	//---------------SETTINGS RELATED--------------//
	void ChangeResolution(int width, int height);
	void SetWindowedFullScreen();
	void SetFullScreen();
	//---------------------------------------------//

	void RequestExit();

	void HandleSignal(Umi::Signal signal);
	void HandleError(Umi::ErrorCode errorCode);

	void LoadState(Umi::StringID stateId);

	bool ProcessMessages();

public:
	template<typename T>
	void CreateStateDescription(Umi::StringID stateId, Umi::PrefabRequest prefabRequest)
	{
		stateRegistry.Register(stateId,
			Umi::StateDescription{
				prefabRequest,
				[](Umi::EngineContext& engineContext) {
					return std::make_unique<T>(engineContext);
				}
			}
		);
	}


	void Run();

	Editor(HINSTANCE hInstance, const char* title);
	~Editor();
};

