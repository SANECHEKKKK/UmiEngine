module;
#define NOMINMAX
#include <windows.h>
#include <algorithm>

#pragma comment (lib, "winmm.lib")
module Editor;

import Time;
import Settings;
import EditorContext;
import TextureManager;
import Transform;
import Camera;

//#include <Signal/Signal.h>
//#include <GameState/GameState.h>

//#include <Tag/TagComponent.h>

////----------SYSTEM EVENTS----------//
//#include <Events/ResolutionChange/ResolutionChange.h>
//#include <Events/LoadState/LoadState.h>
//#include <Events/OpenSettings/OpenSettings.h>
//#include <Events/QuitGame/QuitGame.h>
//#include <Events/ExitToMainMenu/ExitToMainMenu.h>
////--------------------------------//


using namespace Umi;

Editor::Editor(HINSTANCE hInstance, const char* title)
	: window(hInstance, title, engineContext),
	//imGuiManager(window.GetHWND(), window.GetGraphics().GetImguiInitInfo(), engineContext, editorContext, window.GetGraphics().GetGraphicsContext()),
	textureManager(window.GetGraphics().GetGraphicsContext())
{
	auto e = registry.CreateEntity();
	auto textureid = textureManager.LoadTexture("Assets/Textures/MainMenu/TitleScreenBG.png");
	registry.AddComponent<Texture>(e, { textureid });
	registry.AddComponent<Transform>(e, Transform());


	auto a = registry.CreateEntity();
	registry.AddComponent<Transform>(a, Transform());
	registry.AddComponent<Camera>(a, Camera());
}

void Editor::Run()
{
	LARGE_INTEGER frequency, lastTime, currentTime;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&lastTime);

	float accumulator = 0.0f;

	MSG msg = {};
	while (isRunning)
	{
		// ── 1. Drain the ENTIRE OS message queue before touching the game ──
		//    Draining one-per-frame can stall the loop under heavy WM traffic.
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				RequestExit();
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		if (!isRunning)
			break;

		// ── 2. Timing ──────────────────────────────────────────────────────
		QueryPerformanceCounter(&currentTime);

		float rawDelta = static_cast<float>(
			currentTime.QuadPart - lastTime.QuadPart) /
			static_cast<float>(frequency.QuadPart);
		lastTime = currentTime;

		// Spike guard: clamp runaway deltas (debugger pause, OS preemption).
		float deltaTime = std::min(rawDelta, kMaxDeltaTime);
		accumulator = std::min(accumulator + deltaTime, kMaxDeltaTime); // Prevent spiral of death from accumulating too much time.

		// ── 3. Frame ───────────────────────────────────────────────────────
		FrameTick(deltaTime, accumulator);

		// ── 4. Frame limiter (sleep, not spin) ────────────────────────────
		//    Only applied when VSync is off; VSync already throttles us.
		if (!engineContext.settings.isVSyncEnabled())
		{
			const float targetFrameTime = 1.0f / engineContext.settings.getMaxFps();

			QueryPerformanceCounter(&currentTime);
			float elapsed = static_cast<float>(
				currentTime.QuadPart - lastTime.QuadPart) /
				static_cast<float>(frequency.QuadPart);

			float sleepSeconds = targetFrameTime - elapsed;
			if (sleepSeconds > 0.002f)
			{
				// Sleep(1) undershoots by ~1-2 ms; leave a 2 ms margin
				// and spin-wait only the last tiny slice.
				DWORD sleepMs = static_cast<DWORD>((sleepSeconds - 0.002f) * 1000.0f);
				if (sleepMs > 0)
					Sleep(sleepMs);

				// Precise spin for the remaining 2 ms margin
				do { QueryPerformanceCounter(&currentTime); } while (static_cast<float>(currentTime.QuadPart - lastTime.QuadPart) /
					static_cast<float>(frequency.QuadPart) < targetFrameTime);
			}
		}
	}
}

void Editor::FrameTick(float deltaTime, float& accumulator)
{
	//inputManager.keyboard.Update();
	//inputManager.mouse.Update();

	//HandleSignal(HandleInput());

	accumulator += deltaTime;
	//accumulator = std::min(accumulator + deltaTime, kMaxDeltaTime);
	while (accumulator >= kFixedStep)
	{
		//FixedUpdate(kFixedStep);
		accumulator -= kFixedStep;
	}

	const float alpha = accumulator / kFixedStep;

	//HandleSignal(Update(deltaTime));

	window.GetGraphics().FrameStart();
	window.GetGraphics().Render();
	//imGuiManager.Render();
	window.GetGraphics().FrameEnd();
	//window.GetGraphics().Clear();
	//Render(alpha);
	//imGuiManager.DrawBegin();
	//RenderImGui();
	//imGuiManager.DrawEnd();
	//window.GetGraphics().Present();

	//HandleSignal(HandleEvents());
}