module;
#include <Windows.h>

#include <memory>
#include <vector>
#include <functional>

export module Editor;
import Window;
import EngineContext;

//#include <Window/Window.h>
//
////----------ENGINE CONTEXT----------//
//#include <EngineContext/EngineContext.h>
import Settings;
//#include <Rendering/Renderer.h>
//#include <Texture/TextureManager.h>
//#include <Prefab/PrefabManager.h>
//#include <LoadManager/LoadManager.h>
//#include <Input/InputManager.h>
//#include <Model/ModelManager.h>
//#include <Error/ErrorManager.h>
////----------------------------------//

import EditorContext;

//#include <Prefab/PrefabRequest.h>
//#include <StateRegistry/StateRegistry.h>
import Registry;
//
//#include <Rendering/RenderSystem.h>
//#include <Ui/UiSystem.h>

import ImGuiManager;

export namespace Umi
{
	class Editor
	{
	private:
		bool isRunning = true;

		static constexpr float kMaxDeltaTime = 0.25f; // 250 ms
		static constexpr float kFixedStep = 1.0f / 120.0f;

		Umi::Registry registry;
		Umi::EditorContext editorContext;

		Umi::Settings settings;
		//Umi::TextureManager textureManager;
		//Umi::InputManager inputManager{ settings };
		//Umi::EngineContext engineContext{ settings, textureManager, renderer, prefabManager, loadManager, inputManager, registry, uiSystem, modelManager, errorManager };
		Umi::EngineContext engineContext{ settings };
		Umi::Window window;
		//Umi::Renderer renderer;
		//Umi::ImGuiManager imGuiManager;
		//Umi::PrefabManager prefabManager{ engineContext };
		//Umi::LoadManager loadManager{ engineContext };
		//Umi::ModelManager modelManager;
		//Umi::ErrorManager errorManager{ registry };
		Umi::ImGuiManager imGuiManager;

		//Umi::RenderSystem renderSystem{ registry, renderer };
		//Umi::UiSystem uiSystem{ registry };


		//std::vector<std::unique_ptr<Umi::GameState>> gameStack;

		//Umi::PrefabRequest bootPrefabs{
			//.folders = { "LoadingState", "SettingsState" }
		//};

		bool bootPrefabsLoaded = false;

		LARGE_INTEGER currentTime;
		LARGE_INTEGER lastTime;
		LARGE_INTEGER frequency;

		float elapsed;

		void RequestExit() { isRunning = false; }

		void FrameTick(float deltaTime, float& accumulator);

		//void Render();
		
	public:

		void Run();

		Editor(HINSTANCE hInstance, const char* title);
		~Editor() = default;
	};
}