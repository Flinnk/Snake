#pragma warning(disable:4996)
#include <string>
#include <Math.h>
#include <time.h>
#include <GameScene.h>
#include <IntroScene.h>
#include <Constants.h>
#include <Audio.h>
#include <Core.h>
#include <stb/stb_truetype.h>
#include <Engine.h>

void UpdateInput(CInput* InInput)
{

	for (int i = 0; i < ENUM_TO_UINT(EInputDevices::TOTAL); ++i)
	{
		SInputDevice&  Device = InInput->Devices[i];
		for (int j = 0; j < ENUM_TO_UINT(EInputKeys::TOTAL); ++j)
		{
			Device.Keys[j].PreviousState = Device.Keys[j].CurrentState;
		}
	}

	InInput->Devices[ENUM_TO_UINT(EInputDevices::KEYBOARD)].Keys[ENUM_TO_UINT(EInputKeys::UP)].CurrentState = (GetAsyncKeyState(VK_UP) & 0x8000) != 0 || (GetAsyncKeyState('W') & 0x8000) != 0;
	InInput->Devices[ENUM_TO_UINT(EInputDevices::KEYBOARD)].Keys[ENUM_TO_UINT(EInputKeys::DOWN)].CurrentState = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0 || (GetAsyncKeyState('S') & 0x8000) != 0;
	InInput->Devices[ENUM_TO_UINT(EInputDevices::KEYBOARD)].Keys[ENUM_TO_UINT(EInputKeys::LEFT)].CurrentState = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0 || (GetAsyncKeyState('A') & 0x8000) != 0;
	InInput->Devices[ENUM_TO_UINT(EInputDevices::KEYBOARD)].Keys[ENUM_TO_UINT(EInputKeys::RIGHT)].CurrentState = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0 || (GetAsyncKeyState('D') & 0x8000) != 0;
	InInput->Devices[ENUM_TO_UINT(EInputDevices::KEYBOARD)].Keys[ENUM_TO_UINT(EInputKeys::START)].CurrentState = (GetAsyncKeyState(VK_RETURN) & 0x8000) != 0;

	DWORD Result;
	XINPUT_STATE State;
	ZeroMemory(&State, sizeof(XINPUT_STATE));

	// Simply get the state of the controller from XInput.
	Result = XInputGetState(0, &State);

	if (Result == ERROR_SUCCESS)
	{
		// Controller is connected 
		InInput->Devices[ENUM_TO_UINT(EInputDevices::GAMEPAD)].Keys[ENUM_TO_UINT(EInputKeys::UP)].CurrentState = State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
		InInput->Devices[ENUM_TO_UINT(EInputDevices::GAMEPAD)].Keys[ENUM_TO_UINT(EInputKeys::DOWN)].CurrentState = State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
		InInput->Devices[ENUM_TO_UINT(EInputDevices::GAMEPAD)].Keys[ENUM_TO_UINT(EInputKeys::LEFT)].CurrentState = State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
		InInput->Devices[ENUM_TO_UINT(EInputDevices::GAMEPAD)].Keys[ENUM_TO_UINT(EInputKeys::RIGHT)].CurrentState = State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
		InInput->Devices[ENUM_TO_UINT(EInputDevices::GAMEPAD)].Keys[ENUM_TO_UINT(EInputKeys::START)].CurrentState = State.Gamepad.wButtons & XINPUT_GAMEPAD_START;

	}
	else
	{
		// Controller is not connected 
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ShowSystemErrorMessage(const char* Message);

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
	srand(time(NULL));
	State GameState;
	
	unsigned int IntroSize = sizeof(IntroScene);
	unsigned int GameSize = sizeof(GameScene);

	if (!InitializeWindow(hInstance, &GameState.WinVariables))
	{
		ShowSystemErrorMessage("Failed to create window");
		return 0;
	}

	SetWindowLongPtr(GameState.WinVariables.WindowHandle, GWLP_USERDATA, (LONG_PTR)&GameState);

	if (!Engine.Renderer.Initialize(GameState.WinVariables.WindowHandle, WINDOW_WIDTH, WINDOW_HEIGHT))
	{
		Engine.Renderer.Release();
		return 0;
	}

	if (!Engine.AudioManager.Initialize())
		ShowSystemErrorMessage("Failed to initialize AudioSystem");

	CFont* TextFont = Engine.ResourceManager.LoadAndRetrieveFont("Boxy-Bold.ttf", 160, 2048, 2048);
	
	LARGE_INTEGER Frequency;
	LARGE_INTEGER LastCounter;
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&LastCounter);
	float ElapsedTime = 0;

	MSG Message = {};
	Vector4 ClearColor = Vector4(0.0, 0.0, 0.0, 1.0);

	GameState.CurrentScene = new IntroScene();
	GameState.CurrentScene->Enter();
	SceneIdentifier CurrentSceneIdentifier = SceneIdentifier::INTRO;
	SceneIdentifier NextSceneIdentifier = SceneIdentifier::INTRO;

	while (bRun && !Engine.bClose)
	{
		while (PeekMessage(&Message, GameState.WinVariables.WindowHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		if (GameState.HasFocus)//If we dont have focus we should put the process to sleep to not waste cpu
		{
			UpdateInput(&Engine.Input);

			Engine.Renderer.Clear(ClearColor);
			Engine.Renderer.Begin();

			if (NextSceneIdentifier != CurrentSceneIdentifier)
			{
				GameState.CurrentScene->Exit();
				delete GameState.CurrentScene;
				CurrentSceneIdentifier = NextSceneIdentifier;
				switch (CurrentSceneIdentifier)
				{
				case SceneIdentifier::INTRO:
				{
					GameState.CurrentScene = new IntroScene();
					break;
				}
				case SceneIdentifier::GAME:
				{
					GameState.CurrentScene = new GameScene();
					break;
				}
				}
				GameState.CurrentScene->Enter();
			}

			NextSceneIdentifier = GameState.CurrentScene->Update(ElapsedTime);

			Engine.Renderer.End();
			Engine.Renderer.Present();

			LARGE_INTEGER CurrentCounter;
			QueryPerformanceCounter(&CurrentCounter);

			LONGLONG ElapsedCounter = CurrentCounter.QuadPart - LastCounter.QuadPart;
			ElapsedTime = (float(ElapsedCounter) / float(Frequency.QuadPart));
			LastCounter = CurrentCounter;

			char Buffer[2048];
			snprintf(Buffer, sizeof(Buffer), "Snake | m/s: %s | fps: %s", std::to_string(ElapsedTime*1000.0f).c_str(), std::to_string((int)trunc(1 / (ElapsedTime))).c_str());
			SetWindowText(GameState.WinVariables.WindowHandle, Buffer);
		}

	}
	Engine.ResourceManager.ReleaseFont("Boxy-Bold.ttf");

	if (GameState.CurrentScene)
	{
		GameState.CurrentScene->Exit();
		delete GameState.CurrentScene;
	}

	Engine.Renderer.Release();
	Engine.AudioManager.Release();
	return 0;
}

void ShowSystemErrorMessage(const char* Message)
{
	MessageBox(NULL, Message, "Error", MB_OK | MB_ICONERROR);
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
			GetEngine()->AudioManager.StartEngine();
		}
		break;
	}
	case WM_KILLFOCUS:
	{
		State* GameState = (State*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (GameState)
		{
			GameState->HasFocus = false;
			GetEngine()->AudioManager.StopEngine();
		}
		break;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}