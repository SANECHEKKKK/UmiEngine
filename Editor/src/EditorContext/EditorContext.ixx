export module EditorContext;

import Entity;

export namespace Umi
{
	enum class PlayState { Edit, Play, Paused };
	
	struct EditorContext
	{
		Entity selectedEntity { INVALID_ENTITY };
		
		PlayState playState = PlayState::Edit;
		bool requestPlay = false, requestStop = false, requestPauseToggle = false;

		Entity mainCameraEntity { INVALID_ENTITY };
	};
}