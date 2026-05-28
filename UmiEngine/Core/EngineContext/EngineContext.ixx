export module EngineContext;

import Registry;
import Settings;
import TextureManager;
import ModelManager;

export namespace Umi
{
	struct EngineContext
	{
		Registry& registry;
		Settings& settings;
		TextureManager& textureManager;
		ModelManager& modelManager;
	
		EngineContext(Registry& registry, Settings& settings, TextureManager& textureManager, ModelManager& modelManager) : registry(registry), settings(settings), textureManager(textureManager), modelManager(modelManager) {};
		EngineContext(const EngineContext&) = delete;
		EngineContext& operator=(const EngineContext&) = delete;
		~EngineContext() = default;
	};
}