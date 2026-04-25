#include <EngineContext/EngineContext.h>
#include <Settings/Settings.h>
#include <Texture/TextureManager.h>
#include <Rendering/Renderer.h>
#include <Prefab/PrefabManager.h>
#include <LoadManager/LoadManager.h>
#include <Input/InputManager.h>
#include <Registry/Registry.h>
#include <Ui/UiSystem.h>

Umi::EngineContext::EngineContext(Settings& settings, TextureManager& textureManager, Renderer& renderer, PrefabManager& prefabManager, LoadManager& loadManager, InputManager& inputManager, Registry& registry, UiSystem& uiSystem, ModelManager& modelManager, ErrorManager& errorManager)
	: settings(settings), textureManager(textureManager), renderer(renderer), prefabManager(prefabManager), loadManager(loadManager), inputManager(inputManager), registry(registry), uiSystem(uiSystem), modelManager(modelManager), errorManager(errorManager) {}
