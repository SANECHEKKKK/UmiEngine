export module EngineContext;

import Settings;

export namespace Umi
{
	struct EngineContext
	{
		//Registry& registry;

		Settings& settings;
		//TextureManager& textureManager;
		//Renderer& renderer;
		//PrefabManager& prefabManager;
		//LoadManager& loadManager;
		//InputManager& inputManager;
		//UiSystem& uiSystem;
		//ModelManager& modelManager;
		//ErrorManager& errorManager;
	
		EngineContext(Settings& settings) : settings(settings) {};
		//	TextureManager& textureManager,
		//	Renderer& renderer,
		//	PrefabManager& prefabManager,
		//	LoadManager& loadManager,
		//	InputManager& inputManager,
		//	Registry& registry,
		//	UiSystem& uiSystem,
		//	ModelManager& modelManager,
		//	ErrorManager& errorManager);
		EngineContext(const EngineContext&) = delete;
		EngineContext& operator=(const EngineContext&) = delete;
		~EngineContext() = default;
	};
}