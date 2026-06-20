module;
#include <EngineApi/EngineApi.h>
export module EngineContext;

export import Registry;
export import Settings;
export import TextureManager;
export import ModelManager;
export import InputManager;
export import LevelManager;

export namespace Umi
{
	struct ENGINE_API EngineContext
	{
		Registry& registry;
		Settings& settings;
		TextureManager& textureManager;
		ModelManager& modelManager;
		InputManager& inputManager;
		LevelManager& levelManager;
	
		EngineContext(Registry& registry, Settings& settings, TextureManager& textureManager, ModelManager& modelManager, InputManager& inputManager, LevelManager& levelmanager) 
		: registry(registry), settings(settings), textureManager(textureManager), modelManager(modelManager), inputManager(inputManager), levelManager(levelmanager) {};
		EngineContext(const EngineContext&) = delete;
		EngineContext& operator=(const EngineContext&) = delete;
		~EngineContext() = default;
	};
}