export module EngineContext;

import Registry;
import Settings;
import TextureManager;

export namespace Umi
{
	struct EngineContext
	{
		Registry& registry;
		Settings& settings;
		TextureManager& textureManager;
	
		EngineContext(Registry& registry, Settings& settings, TextureManager& textureManager) : registry(registry), settings(settings), textureManager(textureManager) {};
		EngineContext(const EngineContext&) = delete;
		EngineContext& operator=(const EngineContext&) = delete;
		~EngineContext() = default;
	};
}