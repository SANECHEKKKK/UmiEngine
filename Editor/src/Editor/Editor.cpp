#include "Editor.h"

#include <windows.h>
#include <Time/Time.h>
#include <Signal/Signal.h>
#include <GameState/GameState.h>
#include <Graphics/Graphics.h>

#include <Tag/TagComponent.h>

//-------------States--------------//
#include <LoadingState/LoadingState.h>
#include <MainMenuState/MainMenuState.h>
//---------------------------------//

//----------SYSTEM EVENTS----------//
#include <Events/ResolutionChange/ResolutionChange.h>
#include <Events/LoadState/LoadState.h>
#include <Events/OpenSettings/OpenSettings.h>
#include <Events/QuitGame/QuitGame.h>
#include <Events/ExitToMainMenu/ExitToMainMenu.h>
//--------------------------------//

#pragma comment (lib, "winmm.lib")

using namespace Umi;

Editor::Editor(HINSTANCE hInstance, const char* title)
	: window(hInstance, title, engineContext),
	renderer(&window.GetGraphics(), engineContext),
	imGuiManager(window.GetHWND(), window.GetGraphics().DirectXGetDevice(), window.GetGraphics().DirectXGetDeviceContext(), engineContext.registry, editorContext),
	modelManager(window.GetGraphics().DirectXGetDevice(), window.GetGraphics().DirectXGetDeviceContext())
{
	loadManager.LoadThread(bootPrefabs);

	CreateStateDescription<MainMenuState>(MainMenuStateID,
		{
			.folders = { "MainMenu" }
		});

	gameStack.reserve(10);
	LoadState(MainMenuStateID);
	
	ChangeResolution(1280, 720);
}

Editor::~Editor() = default;

void Editor::Run()
{
	MSG msg;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&lastTime);

#if _DEBUG
	QueryPerformanceCounter(&debugTimeLast);
#endif // !_DEBUG

	LoadState(MainMenuStateID);

	do {
		if (!bootPrefabsLoaded)
		{
			loadManager.Update();
			if (loadManager.IsFinished())
				bootPrefabsLoaded = true;
		}

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				RequestExit();
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			QueryPerformanceCounter(&currentTime);
			Time::deltaTime = (float)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
			lastTime = currentTime;


			if (Time::deltaTime >= (1.0f / engineContext.settings.getMaxFps()))
			{
				LARGE_INTEGER frameStart = currentTime;

				//-------MAIN LOOP UPDATE-------
				FrameTick();
				//------------------------------

#ifdef _DEBUG
				debugFps++;
#endif // !_DEBUG

				Time::deltaTime = 0.0f;
				if (!engineContext.settings.isVSyncEnabled())
				{
					LARGE_INTEGER current;
					float targetTime = 1.0f / engineContext.settings.getMaxFps();

					do {
						QueryPerformanceCounter(&current);
						elapsed = (float)(current.QuadPart - frameStart.QuadPart) / frequency.QuadPart;
					} while (elapsed < targetTime);
				}
			}
			else
			{
				//Yield CPU time to other processes
				Sleep(1);
			}

#ifdef _DEBUG
			UpdateDebugTitle();
#endif // !_DEBUG

		}
	} while ((msg.message != WM_QUIT) && isRunning);
}

#ifdef _DEBUG
void Editor::UpdateDebugTitle()
{
	debugTime = (float)(currentTime.QuadPart - debugTimeLast.QuadPart) / frequency.QuadPart;
	if (debugTime >= 1.0f)
	{
		wsprintf(g_DebugStr, window.GetWindowName());
		wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS: %d", (debugFps));
		SetWindowText(window.GetHWND(), g_DebugStr);
		debugTimeLast = currentTime;
		debugFps = 0;
	}
}
#endif // !_DEBUG

void Editor::FrameTick()
{
	window.GetGraphics().Clear();
	HandleSignal(HandleInput());
	HandleSignal(Update());
	imGuiManager.DrawBegin();
	Render();
	imGuiManager.DrawEnd();
	HandleSignal(HandleEvents());
	window.GetGraphics().Present();
	inputManager.keyboard.Update();
	inputManager.mouse.Update();
}

Signal Editor::HandleInput()
{
	if (!gameStack.empty())
	{
		return gameStack.back()->HandleInput();
	}
	return Signal();
}

Signal Editor::Update()
{
	if (!gameStack.empty())
	{
		return gameStack.back()->Update();
	}
	return Signal();	
}

void Editor::Render()
{
	if (!gameStack.empty())
	{
		for (auto& state : gameStack)
		{
			renderer.SetCamera(state->GetCamera());
			renderSystem.Render(state->GetStateID());
		}
	}
}

Signal Editor::HandleEvents()
{
	for (auto e : registry.View<Umi::ResolutionChange, EventTag>())
	{
		auto& resChange = registry.GetComponent<Umi::ResolutionChange>(e);
		if (resChange.resolution.width > 0 && resChange.resolution.height > 0)
		{
			ChangeResolution(resChange.resolution.width, resChange.resolution.height);
			uiSystem.RecalculateAll(settings.getResolution());

			for (auto& state : gameStack)
			{
				state->GetCamera()->UpdateProjectionMatrix();
			}

			registry.DestroyEntity(e);
		}
	}

	for (auto e : registry.View<Umi::LoadState, EventTag>())
	{
		auto& loadState = registry.GetComponent<Umi::LoadState>(e);
		LoadState(loadState.stateID);
		registry.DestroyEntity(e);

	}

	for (auto e : registry.View<Umi::OpenSettings, EventTag>())
	{
		AddState(SettingsStateID);

		registry.DestroyEntity(e);
	}

	for (auto e : registry.View<Umi::QuitGame, EventTag>())
	{
		RequestExit();
		registry.DestroyEntity(e);
	}

	for (auto e : registry.View<Umi::ExitToMainMenu, EventTag>())
	{
		ClearStack();
		LoadState(MainMenuStateID);
		registry.DestroyEntity(e);
	}

	return Signal();
}

void Editor::AddState(StringID stateId)
{
	if (stateId != StringIDNone)
	{
		gameStack.push_back(stateRegistry.Get(stateId).create(engineContext));
	}
}

void Editor::AddState(std::unique_ptr<GameState> state)
{
	if (state)
	{
		gameStack.push_back(std::move(state));
	}
}

void Editor::PopState()
{
	if (!gameStack.empty())
	{
		gameStack.pop_back();
	}
}

void Editor::ClearStack()
{
	gameStack.clear();
}

void Editor::CloseGame()
{
	isRunning = false;
}

void Editor::ChangeResolution(int width, int height)
{
	window.SetResolution(width, height);
}

void Editor::SetWindowedFullScreen()
{
	window.SetWindowedFullScreen();
}

void Editor::SetFullScreen()
{
	window.SetFullScreen(true);
	//EventManager::Get().ResolutionChanged();
}

void Editor::RequestExit()
{
	isRunning = false;
}

void Editor::HandleSignal(Signal signal)
{
	switch (signal.type)
	{
	case(SignalType::None):
		break;

		//------------------CLOSE------------------//
	case(SignalType::Close):
		PopState();
		break;
	case(SignalType::CloseGame):
		CloseGame();
		break;
		//-----------------------------------------//

	case(SignalType::FinishedLoading):
		ClearStack();
		AddState(stateRegistry.Get(signal.nextState).create(engineContext));
		break;
	case(SignalType::AddState):
		AddState(signal.nextState);
		break;

	case(SignalType::GameError):
		HandleError(signal.errorCode);
		break;
	default:
		break;
	}


	return;
}

void Editor::HandleError(ErrorCode errorCode)
{
	std::string errorMessage = ErrorCodeToString(errorCode);
	MessageBoxW(NULL, std::wstring(errorMessage.begin(), errorMessage.end()).c_str(), L"Error", MB_ICONEXCLAMATION | MB_OK);
}

void Editor::LoadState(StringID stateId)
{
	AddState(std::make_unique<LoadingState>(engineContext, stateId, stateRegistry.Get(stateId).prefabRequest));
}

bool Editor::ProcessMessages()
{
	return window.ProcessMessages();
}