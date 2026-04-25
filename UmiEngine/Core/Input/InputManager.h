#pragma once
#include <Input/Keyboard.h>
#include <Input/Mouse.h>

namespace Umi
{
	class Settings;

	class InputManager
	{
	private:
		Settings& settings;

	public:
		Keyboard keyboard;
		Mouse mouse;
		void UpdateRawInput(UINT msg, WPARAM wParam, LPARAM lParam);

		InputManager(Settings& settings) : settings(settings) {};
		~InputManager() = default;
	};
}