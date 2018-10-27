#include <string>
#include <SnakeMath.h>
#include <SnakeRenderer.h>
#include <SnakeInput.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define TILE_MAP_COLLUMNS 16
#define TILE_MAP_ROWS 12
#define SNAKE_BODY_TOTAL 24

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
	HEAD,
	BODY,
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
	TileMapCoordinate() :Row(0), Column(0) {}

	TileMapCoordinate(unsigned int RowParam, unsigned int ColumnParam) :Row(RowParam), Column(ColumnParam) {}
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


TileMapCoordinate GetRandomPositionForApple(TileMapValue TileMap[TILE_MAP_COLLUMNS][TILE_MAP_ROWS])
{
	TileMapCoordinate CandidateCoordinates[TILE_MAP_ROWS*TILE_MAP_COLLUMNS];
	unsigned int CandidateCount = 0;

	for (int Column = 0; Column < TILE_MAP_COLLUMNS; ++Column)
	{
		for (int Row = 0; Row < TILE_MAP_ROWS; ++Row)
		{
			if (TileMap[Column][Row] == TileMapValue::FLOOR)
			{
				CandidateCoordinates[CandidateCount] = TileMapCoordinate(Row, Column);
				++CandidateCount;
			}
		}
	}

	unsigned int RandomValue = rand() % (CandidateCount - 1);
	return CandidateCoordinates[RandomValue];
}

void MoveSnake(TileMapValue TileMap[TILE_MAP_COLLUMNS][TILE_MAP_ROWS], TileMapCoordinate SnakeBodyParts[SNAKE_BODY_TOTAL], unsigned int& CurrentBodyPartsCount, TileMapCoordinate TargetPosition)
{
	TileMapCoordinate PreviousCoordinate = SnakeBodyParts[0];
	for (int Index = 0; Index < CurrentBodyPartsCount; ++Index)
	{
		if (Index == 0)
		{
			TileMap[SnakeBodyParts[Index].Column][SnakeBodyParts[Index].Row] = TileMapValue::FLOOR;
			SnakeBodyParts[Index] = TargetPosition;
			TileMap[SnakeBodyParts[Index].Column][SnakeBodyParts[Index].Row] = TileMapValue::HEAD;
		}
		else
		{
			TileMap[SnakeBodyParts[Index].Column][SnakeBodyParts[Index].Row] = TileMapValue::FLOOR;
			TileMapCoordinate TmpCoordinate = SnakeBodyParts[Index];
			SnakeBodyParts[Index] = PreviousCoordinate;
			PreviousCoordinate = TmpCoordinate;
			TileMap[SnakeBodyParts[Index].Column][SnakeBodyParts[Index].Row] = TileMapValue::BODY;
		}
	}
}

void ResetGame(TileMapValue TileMap[TILE_MAP_COLLUMNS][TILE_MAP_ROWS], MovementDirection& CurrentDirection, TileMapCoordinate SnakeBodyParts[SNAKE_BODY_TOTAL], unsigned int& CurrentBodyPartsCount)
{
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
	CurrentDirection = MovementDirection::NONE;
	TileMapCoordinate InitialSnakePosition;
	InitialSnakePosition.Row = TILE_MAP_ROWS / 2;
	InitialSnakePosition.Column = TILE_MAP_COLLUMNS / 2;
	SnakeBodyParts[0] = InitialSnakePosition;
	TileMap[InitialSnakePosition.Column][InitialSnakePosition.Row] = TileMapValue::HEAD;

	TileMapCoordinate ApplePosition = GetRandomPositionForApple(TileMap);
	TileMap[ApplePosition.Column][ApplePosition.Row] = TileMapValue::APPLE;

	CurrentBodyPartsCount = 1;
}

void RenderGame(Renderer& Renderer, Vector3& TileSize, TileMapValue TileMap[TILE_MAP_COLLUMNS][TILE_MAP_ROWS])
{
	Renderer.Begin();

	for (int Column = 0; Column < TILE_MAP_COLLUMNS; ++Column)
	{
		for (int Row = 0; Row < TILE_MAP_ROWS; ++Row)
		{
			TileMapValue TileValue = TileMap[Column][Row];
			Vector3 TilePosition(Column*TileSize.X, WINDOW_HEIGHT - TileSize.Y - Row * TileSize.Y, 0);
			Vector4 Color;
			Vector3 Offset(0, 0, 0);
			switch (TileValue)
			{
			case TileMapValue::FLOOR:
				Color = Vector4(0.0, 0.0, 0.0, 1.0);
				break;
			case TileMapValue::WALL:
				Color = Vector4(0.23, 0.15, 0.95, 1.0);
				break;
			case TileMapValue::HEAD:
				Color = Vector4(0, 1, 0, 1.0);
				break;
			case TileMapValue::BODY:
				Color = Vector4(0, 1, 0, 1.0);
				Offset = TileSize * 0.2f;
				break;
			case TileMapValue::APPLE:
				Color = Vector4(1, 0, 0, 1.0);
				Offset = TileSize * 0.3f;
				break;
			default:
				Color = Vector4(1.0, 0.0, 1.0, 1.0);
			}

			Renderer.DrawRectangle(TilePosition, TileSize, Offset, Color);
		}
	}


	Renderer.End();

	Renderer.Present();
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand(time(NULL));

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

	TileMapCoordinate SnakeBodyParts[SNAKE_BODY_TOTAL];
	unsigned int CurrentBodyPartsCount = 1;

	unsigned int Score = 0;

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

	ResetGame(TileMap, CurrentDirection, SnakeBodyParts, CurrentBodyPartsCount);

	float MovementDelay = 0.1f;
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

		//Get Direction
		Vector3 NewPosition = Vector3();
		if (InputManager.Get(InputKeys::LEFT) && CurrentDirection != MovementDirection::RIGHT)
			CurrentDirection = MovementDirection::LEFT;
		else if (InputManager.Get(InputKeys::RIGHT) && CurrentDirection != MovementDirection::LEFT)
			CurrentDirection = MovementDirection::RIGHT;

		if (InputManager.Get(InputKeys::DOWN) && CurrentDirection != MovementDirection::UP)
			CurrentDirection = MovementDirection::DOWN;
		else if (InputManager.Get(InputKeys::UP) && CurrentDirection != MovementDirection::DOWN)
			CurrentDirection = MovementDirection::UP;

		//Execute movement at certain spped
		MovementCounter += ElapsedTime;
		if (CurrentDirection != MovementDirection::NONE && MovementCounter >= MovementDelay)
		{
			TileMapCoordinate TargetPosition = SnakeBodyParts[0];
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

			TileMapValue TargetPositionValue = TileMap[TargetPosition.Column][TargetPosition.Row];
			if (TargetPositionValue == TileMapValue::WALL)
			{
				ResetGame(TileMap, CurrentDirection, SnakeBodyParts, CurrentBodyPartsCount);
			}
			else if (TargetPositionValue == TileMapValue::APPLE)
			{

				if (CurrentBodyPartsCount < SNAKE_BODY_TOTAL)//Increase snake size
				{
					SnakeBodyParts[CurrentBodyPartsCount] = SnakeBodyParts[CurrentBodyPartsCount - 1];
					++CurrentBodyPartsCount;
				}

				MoveSnake(TileMap, SnakeBodyParts, CurrentBodyPartsCount, TargetPosition);

				Score += 100;

				TileMapCoordinate ApplePosition = GetRandomPositionForApple(TileMap);
				TileMap[ApplePosition.Column][ApplePosition.Row] = TileMapValue::APPLE;

			}
			else if (TargetPositionValue == TileMapValue::FLOOR)
			{
				MoveSnake(TileMap,SnakeBodyParts,CurrentBodyPartsCount,TargetPosition);
			}
			else//BODY
			{
				ResetGame(TileMap, CurrentDirection, SnakeBodyParts, CurrentBodyPartsCount);
			}

			MovementCounter = 0.0f;
		}

		RenderGame(Renderer, TileSize, TileMap);

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