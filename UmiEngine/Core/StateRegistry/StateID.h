#pragma once

#include <StringID/StringID.h>

namespace Umi
{
	inline constexpr StringID MainMenuStateID = "MainMenu"_sid;
	inline constexpr StringID PlayStateID = "Play"_sid;
	inline constexpr StringID PauseStateID = "Pause"_sid;
	inline constexpr StringID SettingsStateID = "Settings"_sid;

	struct StateIDComponent
	{
		StringID stateID;
	};
}