export module EditorContext;

import Entity;
import ScriptManager;
import LevelManager;

export namespace Umi
{
	enum class PlayState { Edit, Play, Paused };

	struct EditorContext
	{
		Entity selectedEntity{ INVALID_ENTITY };

		PlayState playState = PlayState::Edit;
		bool requestPlay = false;
		bool requestStop = false;
		bool requestPauseToggle = false;

		bool showColliders = true;

		Entity mainCameraEntity{ INVALID_ENTITY };

		ScriptManager* scriptManager{ nullptr };
		LevelManager* levelManager{ nullptr };
	};
}