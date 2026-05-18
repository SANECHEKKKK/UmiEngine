module;
#include <windows.h>
#include <memory>
module Window;

//#include "UmiEventManager.h"
//#include <EngineContext/EngineContext.h>
//#include <Graphics/Graphics.h>
//#include <Settings/Settings.h>
//#include <Texture/TextureManager.h>
//#include <Input/InputManager.h>
//#include <Resolution/Resolution.h>

using namespace Umi;

Window::Window(HINSTANCE hInstance, const char* title, EngineContext& engineContext)
	: hInstance(hInstance), engineContext(engineContext)
{

	//Resolution bufferResolution = engineContext.settings.getResolution();

	WNDCLASSEX wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = WndProcSetup;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_QUESTION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = windowClassName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	RegisterClassExA(&wcex);

	RegisterClassEx(&wcex);

	//RECT wr = { 0, 0, bufferResolution.width, bufferResolution.height };
	RECT wr = { 0, 0, 1200, 720};
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, FALSE);

	hWnd = CreateWindowEx(
		0, windowClassName, title,
		WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, hInstance, this
	);

	ShowWindow(hWnd, SW_SHOW);

	if (hWnd == nullptr)
	{
		MessageBoxA(nullptr, "Failed to create window", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	//if (engineContext.settings.isWindowedFullScreenMode())
	//{
	//	SetWindowLong(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	//	SetWindowPos(hWnd, HWND_TOP, 0, 0, bufferResolution.width, bufferResolution.height, SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
	//}

	graphics = std::make_unique<GraphicsManager>(hWnd);

	//if (graphics->DirectXGetSwapChain())
	//{
	//	engineContext.textureManager.Init(graphics->DirectXGetDevice(), graphics->DirectXGetDeviceContext());
	//}
}

Window::~Window()
{
	if (hWnd != nullptr)
	{
		graphics->~GraphicsManager();
		DestroyWindow(hWnd);
		UnregisterClass(windowClassName, hInstance);
	}
}

LRESULT CALLBACK Window::WndProcSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE) {
		CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
		Window* window = static_cast<Window*>(cs->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcThunk));
		return window->HandleMessage(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::WndProcThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return window ? window->HandleMessage(hWnd, msg, wParam, lParam)
		: DefWindowProc(hWnd, msg, wParam, lParam);
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Window::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//engineContext.inputManager.UpdateRawInput(msg, wParam, lParam);

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg) {
	case WM_SIZE:
		//Implement resizing
		return 0;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
	{
		POINTS pt = MAKEPOINTS(lParam);
		return 0;
	}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool Window::ProcessMessages()
{
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE)) {
		if (msg.message == WM_QUIT)
			return false;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

void Window::SetResolution(int width, int height)
{
	//engineContext.settings.setResolution(width, height);
	if (hWnd != nullptr) {

		SetWindowLong(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX);

		RECT wr = { 0, 0, width, height };
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, FALSE);
		int windowWidth = wr.right - wr.left;
		int windowHeight = wr.bottom - wr.top;

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);
		int x = (screenWidth - windowWidth) / 2;
		int y = (screenHeight - windowHeight) / 2;

		SetWindowPos(hWnd, HWND_NOTOPMOST, x, y, windowWidth, windowHeight,
			SWP_FRAMECHANGED | SWP_SHOWWINDOW);

		ShowWindow(hWnd, SW_RESTORE);
	}
	//graphics->Reset();

}

void Window::SetWindowedFullScreen()
{
	//engineContext.settings.setWindowedFullScreenMode(true);
	//Resolution bufferResolution = engineContext.settings.getScreenResolution();
	if (hWnd != nullptr) {
		//RECT wr = { 0, 0, bufferResolution.width, bufferResolution.height };
		RECT wr = { 0, 0, 1200, 720 };
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
		SetWindowPos(hWnd, nullptr, 0, 0, wr.right - wr.left, wr.bottom - wr.top,
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		SetWindowLong(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
		//SetWindowPos(hWnd, HWND_TOP, 0, 0, bufferResolution.width, bufferResolution.height, SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
		SetWindowPos(hWnd, HWND_TOP, 0, 0, 1200, 720, SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
		//graphics->Reset();
	}
	//EventManager::Get().ResolutionChanged();

}

void Window::SetFullScreen(bool mode)
{
	//engineContext.settings.setWindowedFullScreenMode(mode);
	//if (hWnd != nullptr) {
	//	graphics->SetFullScreen(mode);
	//}
	//graphics->Reset();
	//EventManager::Get().ResolutionChanged();

}