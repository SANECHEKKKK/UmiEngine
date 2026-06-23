module;
#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <memory>
#include <string>

#include <ImGui/imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#pragma comment (lib, "winmm.lib")
module Editor;


import Time;
import Settings;
import EditorContext;
import TextureManager;
import CameraManager;
import GraphicsManager;
import Transform;
import Camera;
import Keyboard;
import Script;
import ID;

import LevelManager;

using namespace Umi;


Editor::Editor(HINSTANCE hInstance, const char* title)
    : window(hInstance, title, engineContext),
      imGuiManager(window.GetHWND(), window.GetGraphics().GetImguiInitInfo(), engineContext, editorContext,
                   window.GetGraphics()),
      textureManager(window.GetGraphics().GetGraphicsContext()),
      modelManager(textureManager, window.GetGraphics().GetGraphicsContext())
{
    window.SetMessageHook([](HWND h, UINT m, WPARAM w, LPARAM l)
    {
        return ImGui_ImplWin32_WndProcHandler(h, m, w, l) != 0;
    });

    editorContext.scriptManager = &scriptManager;

    scriptManager.LoadGameScripts();
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
        accumulator = std::min(accumulator + deltaTime, kMaxDeltaTime);
        // Prevent spiral of death from accumulating too much time.

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
                do { QueryPerformanceCounter(&currentTime); }
                while (static_cast<float>(currentTime.QuadPart - lastTime.QuadPart) /
                    static_cast<float>(frequency.QuadPart) < targetFrameTime);
            }
        }
    }
}

void Editor::EnterPlay()
{
    levelManager.SaveLevel();
    editorContext.playState = PlayState::Play;
    UseEditorCamera(false);
}

void Editor::ExitPlay()
{
    editorContext.playState = PlayState::Edit;
    editorContext.selectedEntity = INVALID_ENTITY; // entities are about to be recreated
    levelManager.LoadLevel(levelManager.currentLevel.levelPath.string());
    UseEditorCamera(true);
}

void Editor::ProcessPlayRequests()
{
    auto& ec = editorContext;
    if (ec.requestPlay && ec.playState == PlayState::Edit) EnterPlay();
    if (ec.requestStop && ec.playState != PlayState::Edit) ExitPlay();
    if (ec.requestPauseToggle && ec.playState != PlayState::Edit)
        ec.playState = (ec.playState == PlayState::Play) ? PlayState::Paused : PlayState::Play;

    ec.requestPlay = ec.requestStop = ec.requestPauseToggle = false;
}


void Editor::FrameTick(float deltaTime, float& accumulator)
{
    imGuiManager.ProcessDeferred();
    ProcessPlayRequests();

    //HandleSignal(HandleInput());

    if (editorContext.playState == PlayState::Play)
    {
        accumulator += deltaTime;
        //accumulator = std::min(accumulator + deltaTime, kMaxDeltaTime);
        while (accumulator >= kFixedStep)
        {
            //FixedUpdate(kFixedStep);
             scriptManager.Update();
            accumulator -= kFixedStep;
        }
    }
    const float alpha = accumulator / kFixedStep;

    //HandleSignal(Update(deltaTime));

    window.GetGraphics().FrameStart();
    window.GetGraphics().Render();
    window.GetGraphics().StartImguiFrame();
    imGuiManager.Render();
    window.GetGraphics().FrameEnd();
    //window.GetGraphics().Clear();
    //Render(alpha);
    //imGuiManager.DrawBegin();
    //RenderImGui();
    //imGuiManager.DrawEnd();
    //window.GetGraphics().Present();

    //HandleSignal(HandleEvents());
}

void Editor::UseEditorCamera(bool set)
{
    if (set)
        window.GetGraphics().GetCameraManager().UseEditorCamera();
    else
        window.GetGraphics().GetCameraManager().UseGameCamera();
}
