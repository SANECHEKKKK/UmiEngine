#include <Input/InputManager.h>
//#include <Settings/Settings.h>

using namespace Umi;

void InputManager::UpdateRawInput(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATEAPP:
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		keyboard.ProcessMessage(msg, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		int xPos = static_cast<short>(LOWORD(lParam));
		int yPos = static_cast<short>(HIWORD(lParam));

		//if (pt.x >= 0 && pt.x < settings.getResolution().width && pt.y >= 0 && pt.y < settings.getResolution().height)
		//{
			//mouse.OnMouseMove(xPos, yPos);
		//}
		//else
		//{
		//	if (wParam & (MK_LBUTTON | MK_RBUTTON))
		//	{
		//		Services::getMouse().OnMouseMove(pt.x, pt.y);
		//	}
		//}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		//PIZDA(hWnd);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	}
	}
}