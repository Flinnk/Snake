#pragma warning(disable:4996)

#include <string>
#include <Math.h>
#include <time.h>
#include <GameScene.h>
#include <IntroScene.h>
#include <Constants.h>
#include <PlatformTime.h>
#include <File.h>
#include <Log.h>
#include <MemoryManager.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static bool bRun = true;

struct WindowsVariables
{
	HWND WindowHandle;
};

struct State
{
	Scene* CurrentScene;
	WindowsVariables WinVariables;
	bool HasFocus = true;
};

bool InitializeWindow(HINSTANCE hInstance, WindowsVariables* Variables)
{
	WNDCLASS WindowClass = { };
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = hInstance;
	WindowClass.lpszClassName = "Snake";

	if (RegisterClass(&WindowClass) == 0)
	{
		return false;
		return 0;
	}

	//Style WS_OVERLAPPEDWINDOW for windowed mode and &~WS_MAXIMIZEBOX to disable the maximize button and &~WS_THICKFRAME disable resizing
	HWND WindowHandle = CreateWindowEx(0, WindowClass.lpszClassName, "Snake", WS_OVERLAPPEDWINDOW&~WS_MAXIMIZEBOX&~WS_THICKFRAME,
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, hInstance, 0);

	if (WindowHandle == NULL)
	{
		return false;
	}

	Variables->WindowHandle = WindowHandle;

	ShowWindow(WindowHandle, SW_SHOW);

	return true;
}


CEngine Engine;

CEngine* GetEngine()
{
	return &Engine;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (!MemoryManager.Initialize())
	{
		ShowSystemErrorMessage("Failed to allocate game memory.");
		return 0;
	}

	InitializeTime();
	srand(time(NULL));
	State GameState;

	if (!InitializeWindow(hInstance, &GameState.WinVariables))
	{
		ShowSystemErrorMessage("Failed to create Window");
		return 0;
	}

	SetWindowLongPtr(GameState.WinVariables.WindowHandle, GWLP_USERDATA, (LONG_PTR)&GameState);

	if (!Renderer.Initialize(GameState.WinVariables.WindowHandle, WINDOW_WIDTH, WINDOW_HEIGHT))
	{
		ShowSystemErrorMessage("Failed to initialize DirectX");
		Renderer.Release();
		return 0;
	}

	if (!AudioManager.Initialize()) 
	{
		ShowSystemErrorMessage("Failed to initialize XAudio");
		return 0;
	}

	float ElapsedTime = 0;

	MSG Message = {};
	Vector4 ClearColor = Vector4(0.0, 0.0, 0.0, 1.0);

	GameState.CurrentScene = new((IntroScene*)MemoryManager.AllocateSceneMemory(sizeof(IntroScene)))IntroScene();
	GameState.CurrentScene->Enter();
	SceneIdentifier CurrentSceneIdentifier = SceneIdentifier::INTRO;
	SceneIdentifier NextSceneIdentifier = SceneIdentifier::INTRO;
	float LastCounter = GetEllapsedMilliseconds();

	MemoryManager.FreeFrameMemory();
	while (bRun && !Engine.HasRequestExit())
	{
		while (PeekMessage(&Message, GameState.WinVariables.WindowHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		if (GameState.HasFocus)//If we dont have focus we should put the process to sleep to not waste cpu
		{
			InputManager.Update();

			Renderer.Clear(ClearColor);
			Renderer.Begin();

			if (NextSceneIdentifier != CurrentSceneIdentifier)
			{
				GameState.CurrentScene->Exit();
				CurrentSceneIdentifier = NextSceneIdentifier;
				MemoryManager.FreeSceneMemory();
				switch (CurrentSceneIdentifier)
				{
				case SceneIdentifier::INTRO:
				{
					GameState.CurrentScene = new((IntroScene*)MemoryManager.AllocateSceneMemory(sizeof(IntroScene)))IntroScene();
					break;
				}
				case SceneIdentifier::GAME:
				{
					GameState.CurrentScene = new((GameScene*)MemoryManager.AllocateSceneMemory(sizeof(GameScene)))GameScene();
					break;
				}
				}
				GameState.CurrentScene->Enter();
			}

			NextSceneIdentifier = GameState.CurrentScene->Update(ElapsedTime);

			Renderer.End();
			Renderer.Present();
			MemoryManager.FreeFrameMemory();

			float CurrentCounter = GetEllapsedMilliseconds();
			ElapsedTime = (CurrentCounter - LastCounter);
			LastCounter = CurrentCounter;

			char Buffer[2048];
			snprintf(Buffer, sizeof(Buffer), "Snake | m/s: %s | fps: %s", std::to_string(ElapsedTime*1000.0f).c_str(), std::to_string((int)trunc(1 / (ElapsedTime))).c_str());
			SetWindowText(GameState.WinVariables.WindowHandle, Buffer);
		}

	}

	if (GameState.CurrentScene)
	{
		GameState.CurrentScene->Exit();
	}

	AudioManager.Release();
	Renderer.Release();

	MemoryManager.Release();

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		bRun = false;
		return 0;
	case WM_SETFOCUS:
	{
		State* GameState = (State*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (GameState)
		{
			GameState->HasFocus = true;
			if (AudioManager.IsInitialized())
			AudioManager.StartEngine();
		}
		return 0;
	}
	case WM_KILLFOCUS:
	{
		State* GameState = (State*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (GameState)
		{
			GameState->HasFocus = false;
			if (AudioManager.IsInitialized())
				AudioManager.StopEngine();
		}
		return 0;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}