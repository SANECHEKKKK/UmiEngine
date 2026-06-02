module;
#include <windows.h>
export module InputManager;

import Keyboard;
import Mouse;

export namespace Umi
{
	class InputManager
	{
	private:

	public:
		Keyboard keyboard;
		Mouse mouse;

		void UpdateRawInput(UINT msg, WPARAM wParam, LPARAM lParam);

		InputManager() = default;
		~InputManager() = default;
	};
}