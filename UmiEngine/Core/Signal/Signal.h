#pragma once
#include <memory>
#include <StateRegistry/StateID.h>
#include <Error/Error.h>

namespace Umi
{
	class GameState;

	enum class SignalType : int
	{
		None = 0,

		AddState,

		Close,
		CloseGame,

		StartPlay,

		OpenPauseMenu,
		OpenMainMenu,
		OpenSettings,

		Loading,
		FinishedLoading,

		//------------------SETTINGS------------------//
		SetDisplayMode,
		SetWindowFullscreenMode,
		SetFullscreenMode,
		SetWindowedMode,
		SetResolution,
		ToggleVSync,
		SetVolume,
		//SetLanguage,		TODO: Implement language setting
		//--------------------------------------------//

		ButtonPressed,
		DropdownPressed,

		GameError,
	};



	struct Signal
	{
		SignalType type;
		StringID nextState = StringIDNone;											//Optional next state for state transitions
		ErrorCode errorCode = ErrorCode::None;										//Optional error code for error signals

		Signal() : type(SignalType::None) {};										//Default constructor for no action
		Signal(SignalType _state) : type(_state) {};								//Constructor for specific state without next state or loading progress
		Signal(SignalType _state, StringID _nextState)
			: type(_state), nextState(_nextState) {
		};
		
		Signal(Signal const& other) = default;

		Signal(Signal&& other) noexcept
			: type(other.type), nextState(other.nextState), errorCode(other.errorCode)
		{
			other.type = SignalType::None;
		}
		Signal& operator=(Signal&& other) noexcept
		{
			if (this != &other)
			{
				type = other.type;
				nextState = other.nextState;
				errorCode = other.errorCode;
				other.type = SignalType::None;
			}
			return *this;
		}
		~Signal() = default;
	};
}