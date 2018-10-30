#pragma once

#include <Xinput.h>
#define ENUM_TO_UINT(x) static_cast<unsigned int>(x)

enum class InputKeys : UINT
{
	UP = 0,
	DOWN,
	LEFT,
	RIGHT,
	START,
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
	bool CurrentState = false;
	bool PreviousState = false;

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

