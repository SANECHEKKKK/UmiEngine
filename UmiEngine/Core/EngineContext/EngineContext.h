#pragma once

namespace Umi
{
	class Registry;

	class Settings;
	class TextureManager;
	class Renderer;
	class PrefabManager;
	class LoadManager;
	class InputManager;
	class UiSystem;
	class ModelManager;
	class ErrorManager;

	struct EngineContext
	{
		Registry& registry;

		Settings& settings;
		TextureManager& textureManager;
		Renderer& renderer;
		PrefabManager& prefabManager;
		LoadManager& loadManager;
		InputManager& inputManager;
		UiSystem& uiSystem;
		ModelManager& modelManager;
		ErrorManager& errorManager;

		EngineContext(Settings& settings,
			TextureManager& textureManager,
			Renderer& renderer,
			PrefabManager& prefabManager,
			LoadManager& loadManager,
			InputManager& inputManager,
			Registry& registry,
			UiSystem& uiSystem,
			ModelManager& modelManager,
			ErrorManager& errorManager);
		EngineContext(const EngineContext&) = delete;
		EngineContext& operator=(const EngineContext&) = delete;
		~EngineContext() = default;
	};
}