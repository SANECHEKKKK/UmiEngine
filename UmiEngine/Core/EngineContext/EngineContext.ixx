export module EngineContext;

import Registry;
import Settings;
import TextureManager;
import ModelManager;
import InputManager;

export namespace Umi
{
	struct EngineContext
	{
		Registry& registry;
		Settings& settings;
		TextureManager& textureManager;
		ModelManager& modelManager;
		InputManager& inputManager;
	
		EngineContext(Registry& registry, Settings& settings, TextureManager& textureManager, ModelManager& modelManager, InputManager& inputManager) : registry(registry), settings(settings), textureManager(textureManager), modelManager(modelManager), inputManager(inputManager) {};
		EngineContext(const EngineContext&) = delete;
		EngineContext& operator=(const EngineContext&) = delete;
		~EngineContext() = default;
	};
}