module;
#include <EngineApi/EngineApi.h>
export module EngineContext;

export import Registry;
export import Settings;
export import TextureManager;
export import ModelManager;
export import InputManager;

export namespace Umi
{
	struct ENGINE_API EngineContext
	{
		Registry& registry;
		Settings& settings;
		TextureManager& textureManager;
		ModelManager& modelManager;
		InputManager& inputManager;

		EngineContext(Registry& registry, Settings& settings, TextureManager& textureManager, ModelManager& modelManager, InputManager& inputManager)
		: registry(registry), settings(settings), textureManager(textureManager), modelManager(modelManager), inputManager(inputManager) {};
		EngineContext(const EngineContext&) = delete;
		EngineContext& operator=(const EngineContext&) = delete;
		~EngineContext() = default;
	};
}