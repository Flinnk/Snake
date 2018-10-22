#include <string>
#include <SnakeMath.h>
#include <SnakeRenderer.h>
#include <Xinput.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define ENUM_TO_UINT(x) static_cast<unsigned int>(x)

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ShowSystemErrorMessage(const char* Message);

static bool bRun = true;

struct WindowsVariables
{
	HWND WindowHandle;
};

enum class InputKeys : UINT
{
	UP = 0,
	DOWN,
	LEFT,
	RIGHT,
	TOTAL
};

enum class InputDevices
{
	KEYBOARD = 0,
	GAMEPAD,
	TOTAL
};

struct KeyState
{
	bool CurrentState;
	bool PreviousState;
	
	bool Get()
	{
		return CurrentState;
	}

	bool GetUp()
	{
		return PreviousState && !CurrentState;
	}

	bool GetDown()
	{
		return !PreviousState && CurrentState;
	}
};

struct InputDevice
{
	KeyState Keys[ENUM_TO_UINT(InputKeys::TOTAL)];
};

struct Input
{
	InputDevice Devices[2];

	bool Get(InputKeys Key)
	{
		if (Key == InputKeys::TOTAL)
			return false;

		bool DetectedInput = false;
		int DeviceIndex = 0;
		do
		{
			DetectedInput = Devices[DeviceIndex].Keys[ENUM_TO_UINT(Key)].Get();
			++DeviceIndex;
		} while (!DetectedInput && DeviceIndex < ENUM_TO_UINT(InputDevices::TOTAL));
		
		return DetectedInput;
	}

	bool GetUp(InputKeys Key)
	{
		if (Key == InputKeys::TOTAL)
			return false;

		bool DetectedInput = false;
		int DeviceIndex = 0;
		do
		{
			DetectedInput = Devices[DeviceIndex].Keys[ENUM_TO_UINT(Key)].GetUp();
			++DeviceIndex;
		} while (!DetectedInput && DeviceIndex < ENUM_TO_UINT(InputDevices::TOTAL));

		return DetectedInput;
	}

	bool GetDown(InputKeys Key)
	{
		if (Key == InputKeys::TOTAL)
			return false;

		bool DetectedInput = false;
		int DeviceIndex = 0;
		do
		{
			DetectedInput = Devices[DeviceIndex].Keys[ENUM_TO_UINT(Key)].GetDown();
			++DeviceIndex;
		} while (!DetectedInput && DeviceIndex < ENUM_TO_UINT(InputDevices::TOTAL));

		return DetectedInput;
	}
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

	HWND WindowHandle = CreateWindowEx(0, WindowClass.lpszClassName, "Snake", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, hInstance, 0);

	if (WindowHandle == NULL)
	{
		return false;
	}

	Variables->WindowHandle = WindowHandle;

	ShowWindow(WindowHandle, SW_SHOW);

	return true;
}

void UpdateInput(Input* InInput)
{

	for (int i = 0; i < ENUM_TO_UINT(InputDevices::TOTAL); ++i)
	{
		InputDevice Device = InInput->Devices[i];
		for (int j = 0; j < ENUM_TO_UINT(InputKeys::TOTAL); ++j)
		{
			Device.Keys[j].PreviousState = Device.Keys[j].CurrentState;
		}
	}

	InInput->Devices[ENUM_TO_UINT(InputDevices::KEYBOARD)].Keys[ENUM_TO_UINT(InputKeys::UP)].CurrentState = (GetAsyncKeyState(VK_UP) & 0x8000) != 0 || (GetAsyncKeyState('W') & 0x8000) != 0;
	InInput->Devices[ENUM_TO_UINT(InputDevices::KEYBOARD)].Keys[ENUM_TO_UINT(InputKeys::DOWN)].CurrentState = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0 || (GetAsyncKeyState('S') & 0x8000) != 0;
	InInput->Devices[ENUM_TO_UINT(InputDevices::KEYBOARD)].Keys[ENUM_TO_UINT(InputKeys::LEFT)].CurrentState = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0 || (GetAsyncKeyState('A') & 0x8000) != 0;
	InInput->Devices[ENUM_TO_UINT(InputDevices::KEYBOARD)].Keys[ENUM_TO_UINT(InputKeys::RIGHT)].CurrentState = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0 || (GetAsyncKeyState('D') & 0x8000) != 0;

	DWORD Result;
	XINPUT_STATE State;
	ZeroMemory(&State, sizeof(XINPUT_STATE));

	// Simply get the state of the controller from XInput.
	Result = XInputGetState(0, &State);

	if (Result == ERROR_SUCCESS)
	{
		// Controller is connected 
		InInput->Devices[ENUM_TO_UINT(InputDevices::GAMEPAD)].Keys[ENUM_TO_UINT(InputKeys::UP)].CurrentState = State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
		InInput->Devices[ENUM_TO_UINT(InputDevices::GAMEPAD)].Keys[ENUM_TO_UINT(InputKeys::DOWN)].CurrentState = State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
		InInput->Devices[ENUM_TO_UINT(InputDevices::GAMEPAD)].Keys[ENUM_TO_UINT(InputKeys::LEFT)].CurrentState = State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
		InInput->Devices[ENUM_TO_UINT(InputDevices::GAMEPAD)].Keys[ENUM_TO_UINT(InputKeys::RIGHT)].CurrentState = State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
	}
	else
	{
		// Controller is not connected 
	}
}


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WindowsVariables WinVariables;
	Renderer Renderer;
	Input InputManager;

	if (!InitializeWindow(hInstance, &WinVariables))
	{
		ShowSystemErrorMessage("Failed to create window");
		return 0;
	}

	SetWindowLongPtr(WinVariables.WindowHandle, GWLP_USERDATA, (LONG)&InputManager);


	if (!Renderer.Initialize(WinVariables.WindowHandle, WINDOW_WIDTH, WINDOW_HEIGHT))
	{
		Renderer.Release();
		return 0;
	}

	LARGE_INTEGER Frequency;
	LARGE_INTEGER LastCounter;
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&LastCounter);
	float ElapsedTime = 0;

	MSG Message = {};
	Vector3 Direction = Vector3(0, 0, 0);
	Vector3 Position = Vector3(100, 50, 0);
	float Speed = 500.0f;
	Vector4 ClearColor = Vector4(0.0, 0.0, 1.0, 1.0);
	Vector3 RectangleSize = Vector3(100, 100, 0);
	while (bRun)
	{	
		while (PeekMessage(&Message, WinVariables.WindowHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		UpdateInput(&InputManager);

		Renderer.Clear(ClearColor);

		Vector3 NewPosition = Vector3();
		if (InputManager.Get(InputKeys::LEFT))
			Direction.X = -1;
		else if (InputManager.Get(InputKeys::RIGHT))
			Direction.X = 1;
		else
			Direction.X = 0;

		if (InputManager.Get(InputKeys::DOWN))
			Direction.Y = -1;
		else if (InputManager.Get(InputKeys::UP))
			Direction.Y = 1;
		else
			Direction.Y = 0;

		NewPosition = Position + Direction * Speed * ElapsedTime;

		bool CanMove = NewPosition.X > 0 && NewPosition.X + RectangleSize.X < WINDOW_WIDTH && NewPosition.Y > 0 && NewPosition.Y + RectangleSize.Y < WINDOW_HEIGHT;

		if (CanMove)
			Position += Direction * Speed* ElapsedTime;

		Renderer.Begin();

		Renderer.DrawRectangle(Position, RectangleSize, Vector4(1.0, 0.0, 0.0, 1.0));

		Renderer.End();

		Renderer.Present();

		LARGE_INTEGER CurrentCounter;
		QueryPerformanceCounter(&CurrentCounter);

		LONGLONG ElapsedCounter = CurrentCounter.QuadPart - LastCounter.QuadPart;
		ElapsedTime = (float(ElapsedCounter) / float(Frequency.QuadPart));
		LastCounter = CurrentCounter;

		char Buffer[2048];
		snprintf(Buffer, sizeof(Buffer), "Snake | m/s: %s | fps: %s", std::to_string(ElapsedTime*1000.0f).c_str(), std::to_string((int)trunc(1 / (ElapsedTime))).c_str());
		SetWindowText(WinVariables.WindowHandle, Buffer);

	}

	Renderer.Release();
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
		/*case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			Input* InputManager = (Input*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

			unsigned int KeyCode = wParam;
			bool KeyRepeat = (lParam >> 30) == 1;
			if (!KeyRepeat)
			{
				if (KeyCode == VK_UP)
					InputManager->Keyboard.Down.CurrentState = true;
				if (KeyCode == VK_DOWN)
					InputManager->Keyboard.Down.CurrentState = true;
				if (KeyCode == VK_LEFT)
					InputManager->Keyboard.Left.CurrentState = true;
				if (KeyCode == VK_RIGHT)
					InputManager->Keyboard.Right.CurrentState = true;
			}
			return 0;
		}
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			Input* InputManager = (Input*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

			unsigned int KeyCode = wParam;
			if (KeyCode == VK_UP)
				InputManager->Keyboard.Up.CurrentState = false;
			if (KeyCode == VK_DOWN)
				InputManager->Keyboard.Down.CurrentState = false;
			if (KeyCode == VK_LEFT)
				InputManager->Keyboard.Left.CurrentState = false;
			if (KeyCode == VK_RIGHT)
				InputManager->Keyboard.Right.CurrentState = false;
			return 0;
		}*/
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}