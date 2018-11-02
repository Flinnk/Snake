#include <string>
#include <SnakeMath.h>
#include <time.h>
#include <GameScene.h>
#include <IntroScene.h>
#include <Constants.h>
#include <xaudio2.h>

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

static HRESULT FindChunk(HANDLE File, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(File, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

	while (hr == S_OK)
	{
		DWORD dwRead;
		if (0 == ReadFile(File, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		if (0 == ReadFile(File, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		switch (dwChunkType)
		{
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (0 == ReadFile(File, &dwFileType, sizeof(DWORD), &dwRead, NULL))
				hr = HRESULT_FROM_WIN32(GetLastError());
			break;

		default:
			if (INVALID_SET_FILE_POINTER == SetFilePointer(File, dwChunkDataSize, NULL, FILE_CURRENT))
				return HRESULT_FROM_WIN32(GetLastError());
		}

		dwOffset += sizeof(DWORD) * 2;

		if (dwChunkType == fourcc)
		{
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize) return S_FALSE;

	}

	return S_OK;

}


static HRESULT ReadChunkData(HANDLE File, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(File, bufferoffset, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());
	DWORD dwRead;
	if (0 == ReadFile(File, buffer, buffersize, &dwRead, NULL))
		hr = HRESULT_FROM_WIN32(GetLastError());
	return hr;
}

bool LoadAudio(const char* FilePath, IXAudio2* XAudio2, IXAudio2SourceVoice** AudioSource, BYTE** BufferDataPointer)
{
	HANDLE File = CreateFile(
		FilePath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (INVALID_HANDLE_VALUE == File)
		return false;

	if (INVALID_SET_FILE_POINTER == SetFilePointer(File, 0, NULL, FILE_BEGIN))
		return false;

	WAVEFORMATEXTENSIBLE WAVEFORMAT = { 0 };
	XAUDIO2_BUFFER Buffer = { 0 };

	DWORD ChunkSize;
	DWORD ChunkPosition;
	//check the file type, should be fourccWAVE or 'XWMA'
	FindChunk(File, fourccRIFF, ChunkSize, ChunkPosition);
	DWORD filetype;
	ReadChunkData(File, &filetype, sizeof(DWORD), ChunkPosition);
	if (filetype != fourccWAVE)
		return false;

	FindChunk(File, fourccFMT, ChunkSize, ChunkPosition);
	ReadChunkData(File, &WAVEFORMAT, ChunkSize, ChunkPosition);

	//fill out the audio data buffer with the contents of the fourccDATA chunk
	FindChunk(File, fourccDATA, ChunkSize, ChunkPosition);
	BYTE * DataBuffer = new BYTE[ChunkSize];
	ReadChunkData(File, DataBuffer, ChunkSize, ChunkPosition);

	Buffer.AudioBytes = ChunkSize;  //buffer containing audio data
	Buffer.pAudioData = DataBuffer;  //size of the audio buffer in bytes
	Buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
	Buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	IXAudio2SourceVoice* SourceVoice;
	HRESULT hr;
	if (FAILED(hr = XAudio2->CreateSourceVoice(&SourceVoice, (WAVEFORMATEX*)&WAVEFORMAT)))
		return false;

	if (FAILED(hr = SourceVoice->SubmitSourceBuffer(&Buffer)))
		return false;

	*AudioSource = SourceVoice;
	*BufferDataPointer = DataBuffer;
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
	InInput->Devices[ENUM_TO_UINT(InputDevices::KEYBOARD)].Keys[ENUM_TO_UINT(InputKeys::START)].CurrentState = (GetAsyncKeyState(VK_RETURN) & 0x8000) != 0;

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
		InInput->Devices[ENUM_TO_UINT(InputDevices::GAMEPAD)].Keys[ENUM_TO_UINT(InputKeys::START)].CurrentState = State.Gamepad.wButtons & XINPUT_GAMEPAD_START;

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
	srand(time(NULL));
	GameScene Game;
	IntroScene Intro;
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
	Vector4 ClearColor = Vector4(0.0, 0.0, 0.0, 1.0);

	Intro.Enter();
	SceneIdentifier CurrentSceneIdentifier = SceneIdentifier::INTRO;
	SceneIdentifier NextSceneIdentifier = SceneIdentifier::INTRO;
	Scene* CurrentScene = &Intro;


	//Temp Audio Stuff
	IXAudio2* XAudio2;
	IXAudio2MasteringVoice* MasterVoice;
	HRESULT Result = XAudio2Create(&XAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(Result))
		ShowSystemErrorMessage("Failed to create XAudio2");

	if (FAILED(Result = XAudio2->CreateMasteringVoice(&MasterVoice)))
		ShowSystemErrorMessage("Failed to create MasterVoice");

	IXAudio2SourceVoice* SourceVoice = nullptr;
	BYTE * DataBuffer = nullptr;

	if (!LoadAudio("zone-of-danger.wav", XAudio2, &SourceVoice, &DataBuffer))
		ShowSystemErrorMessage("Failed to load audio");

	if (SourceVoice) 
	{
		SourceVoice->SetVolume(0.1, 0);
		SourceVoice->Start(0, 0);
	}

	while (bRun)
	{
		while (PeekMessage(&Message, WinVariables.WindowHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		UpdateInput(&InputManager);

		Renderer.Clear(ClearColor);
		Renderer.Begin();

		if (NextSceneIdentifier != CurrentSceneIdentifier)
		{
			CurrentScene->Exit();
			CurrentSceneIdentifier = NextSceneIdentifier;
			switch (CurrentSceneIdentifier)
			{
			case SceneIdentifier::INTRO:
			{
				CurrentScene = &Intro;
				break;
			}
			case SceneIdentifier::GAME:
			{
				CurrentScene = &Game;
				break;
			}
			}
			CurrentScene->Enter();
		}

		NextSceneIdentifier = CurrentScene->Update(ElapsedTime, InputManager, Renderer);

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

	Game.Exit();

	Renderer.Release();
	if (SourceVoice)
	{
		SourceVoice->Stop();
		SourceVoice->DestroyVoice();
	}
	if (DataBuffer)
		delete DataBuffer;
	XAudio2->Release();
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