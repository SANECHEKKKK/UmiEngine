#pragma once
#include <Registry/Registry.h>
#include <Rendering/RenderSystem.h>
#include <Signal/Signal.h>
#include <EngineContext/EngineContext.h>
#include <Prefab/PrefabRequest.h>
#include <Entity/Entity.h>
#include <Prefab/Prefab.h>
#include <Camera/Camera.h>
#include <Ui/UiSystem.h>

namespace Umi
{
	class Renderer;

	class GameState
	{
	protected:
		StringID stateID;

		EngineContext& engineContext;

		Camera mainCamera{ engineContext };

		//--------------------------Prefab Creation Helpers------------------------//
		Entity CreateFromPrefab(const PrefabName& prefabName);
		Entity CreateUi(const PrefabName& prefabName);

		void Delete(Entity e);
		//-------------------------------------------------------------------------//

	public:
		inline StringID GetStateID() const noexcept { return stateID; };

		Camera* GetCamera() noexcept { return &mainCamera; };

		virtual Signal HandleInput() = 0;
		virtual Signal Update() = 0;

		GameState(EngineContext& engineContext);
		virtual ~GameState();
	};
}