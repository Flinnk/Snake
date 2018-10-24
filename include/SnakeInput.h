#pragma once

#include <Xinput.h>
#define ENUM_TO_UINT(x) static_cast<unsigned int>(x)

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