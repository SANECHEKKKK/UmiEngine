module;
#include <windows.h>
#include <string>
#include <memory>
export module Window;

import GraphicsManager;
import EngineContext;

namespace Umi
{
	export class Window
	{
	private:
		static LRESULT CALLBACK WndProcSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK WndProcThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		std::unique_ptr<GraphicsManager> graphics;
		HINSTANCE hInstance;
		HWND hWnd = nullptr;
		static constexpr const char* windowClassName = "MyWindowClass";

		EngineContext& engineContext;

	public:
		bool ProcessMessages();
		const HWND& GetHWND() const { return hWnd; }

		void SetResolution(int width, int height);
		void SetWindowedFullScreen();
		void SetFullScreen(bool mode);

		const char* GetWindowName() { return windowClassName; };

		GraphicsManager& GetGraphics() { return *graphics; };


		Window(HINSTANCE hInstance, const char* title, EngineContext& engineContext);
		~Window();
	};
}