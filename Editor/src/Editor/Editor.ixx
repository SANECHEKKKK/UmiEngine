module;
#include <Windows.h>

#include <memory>
#include <vector>
#include <functional>
export module Editor;

import Window;
import EngineContext;

import Settings;

import EditorContext;
import Registry;

import TextureManager;
import ModelManager;
import ImGuiManager;
import InputManager;
import ScriptManager;

export namespace Umi
{
	class Editor
	{
	private:
		bool isRunning = true;

		static constexpr float kMaxDeltaTime = 0.25f; // 250 ms
		static constexpr float kFixedStep = 1.0f / 120.0f;
		Umi::EngineContext engineContext{ registry, settings, textureManager, modelManager, inputManager };
		Umi::Settings settings;

		Umi::Window window;

		Umi::EditorContext editorContext;

		Umi::Registry registry;

		Umi::TextureManager textureManager;
		Umi::ModelManager modelManager;
		Umi::ImGuiManager imGuiManager;
		Umi::InputManager inputManager;
		Umi::ScriptManager scriptManager { engineContext };

		bool bootPrefabsLoaded = false;

		LARGE_INTEGER currentTime;
		LARGE_INTEGER lastTime;
		LARGE_INTEGER frequency;

		float elapsed;

		void RequestExit() { isRunning = false; }

		void FrameTick(float deltaTime, float& accumulator);
		
	public:

		void Run();

		Editor(HINSTANCE hInstance, const char* title);
		~Editor() = default;
	};
}