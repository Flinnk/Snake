#include <string>
#include <SnakeMath.h>
#include <SnakeRenderer.h>
#include <SnakeInput.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define TILE_MAP_COLLUMNS 16
#define TILE_MAP_ROWS 12

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ShowSystemErrorMessage(const char* Message);

static bool bRun = true;

struct WindowsVariables
{
	HWND WindowHandle;
};

enum class TileMapValue
{
	FLOOR,
	WALL,
	SNAKE,
	APPLE
};

enum class MovementDirection
{
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct TileMapCoordinate
{
	unsigned int Row;
	unsigned int Column;
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

	Vector4 ClearColor = Vector4(0.0, 0.0, 0.0, 1.0);
	MovementDirection CurrentDirection = MovementDirection::NONE;
	TileMapCoordinate SnakePosition;
	SnakePosition.Row = TILE_MAP_ROWS / 2;
	SnakePosition.Column = TILE_MAP_COLLUMNS / 2;

	Vector3 TileSize(WINDOW_WIDTH / TILE_MAP_COLLUMNS, WINDOW_HEIGHT / TILE_MAP_ROWS, 0);
	TileMapValue TileMap[TILE_MAP_COLLUMNS][TILE_MAP_ROWS];
	for (int Column = 0; Column < TILE_MAP_COLLUMNS; ++Column)
	{
		for (int Row = 0; Row < TILE_MAP_ROWS; ++Row)
		{
			if (Row == 0 || Column == 0 || Row == TILE_MAP_ROWS - 1 || Column == TILE_MAP_COLLUMNS - 1)
				TileMap[Column][Row] = TileMapValue::WALL;
			else
				TileMap[Column][Row] = TileMapValue::FLOOR;
		}
	}

	TileMap[SnakePosition.Column][SnakePosition.Row] = TileMapValue::SNAKE;


	float MovementDelay = 0.5f;
	float MovementCounter = 0.0f;

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
			CurrentDirection = MovementDirection::LEFT;
		else if (InputManager.Get(InputKeys::RIGHT))
			CurrentDirection = MovementDirection::RIGHT;

		if (InputManager.Get(InputKeys::DOWN))
			CurrentDirection = MovementDirection::DOWN;
		else if (InputManager.Get(InputKeys::UP))
			CurrentDirection = MovementDirection::UP;

		MovementCounter += ElapsedTime;
		if (CurrentDirection != MovementDirection::NONE && MovementCounter>= MovementDelay)
		{
			TileMapCoordinate TargetPosition = SnakePosition;
			switch (CurrentDirection)
			{

			case MovementDirection::UP:
				TargetPosition.Row -= 1;
				break;
			case MovementDirection::DOWN:
				TargetPosition.Row += 1;
				break;
			case MovementDirection::LEFT:
				TargetPosition.Column -= 1;
				break;
			case MovementDirection::RIGHT:
				TargetPosition.Column += 1;
				break;
			}
			bool CanMove = TargetPosition.Column >= 1 && TargetPosition.Column < TILE_MAP_COLLUMNS-1 && TargetPosition.Row >= 1 && TargetPosition.Row < TILE_MAP_ROWS-1;
			if (CanMove)
			{
				TileMap[SnakePosition.Column][SnakePosition.Row] = TileMapValue::FLOOR;
				TileMap[TargetPosition.Column][TargetPosition.Row] = TileMapValue::SNAKE;
				SnakePosition = TargetPosition;
			}

			MovementCounter = 0.0f;
		}


		Renderer.Begin();

		for (int Column = 0; Column < TILE_MAP_COLLUMNS; ++Column)
		{
			for (int Row = 0; Row < TILE_MAP_ROWS; ++Row)
			{
				TileMapValue TileValue = TileMap[Column][Row];
				Vector3 TilePosition(Column*TileSize.X, WINDOW_HEIGHT - TileSize.Y - Row * TileSize.Y, 0);
				Vector4 Color;
				switch (TileValue)
				{
				case TileMapValue::FLOOR:
					Color = Vector4(0.0, 0.0, 0.0, 1.0);
					break;
				case TileMapValue::WALL:
					Color = Vector4(0.23, 0.15, 0.95, 1.0);
					break;
				case TileMapValue::SNAKE:
					Color = Vector4(0, 1, 0, 1.0);
					break;
				default:
					Color = Vector4(1.0, 0.0, 1.0, 1.0);
				}

				Renderer.DrawRectangle(TilePosition, TileSize, Color);
			}
		}


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