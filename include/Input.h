#pragma once

#include <Xinput.h>
#define ENUM_TO_UINT(x) static_cast<unsigned int>(x)

enum class EInputKeys : UINT
{
	UP = 0,
	DOWN,
	LEFT,
	RIGHT,
	START,
	TOTAL
};

enum class EInputDevices
{
	KEYBOARD = 0,
	GAMEPAD,
	TOTAL
};

struct SKeyState
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

struct SInputDevice
{
	SKeyState Keys[ENUM_TO_UINT(EInputKeys::TOTAL)];
};

class CInput
{
public:
	SInputDevice Devices[2];
	bool Get(EInputKeys Key)
	{
		if (Key == EInputKeys::TOTAL)
			return false;

		bool DetectedInput = false;
		int DeviceIndex = 0;
		do
		{
			DetectedInput = Devices[DeviceIndex].Keys[ENUM_TO_UINT(Key)].Get();
			++DeviceIndex;
		} while (!DetectedInput && DeviceIndex < ENUM_TO_UINT(EInputDevices::TOTAL));

		return DetectedInput;
	}

	bool GetUp(EInputKeys Key)
	{
		if (Key == EInputKeys::TOTAL)
			return false;

		bool DetectedInput = false;
		int DeviceIndex = 0;
		do
		{
			DetectedInput = Devices[DeviceIndex].Keys[ENUM_TO_UINT(Key)].GetUp();
			++DeviceIndex;
		} while (!DetectedInput && DeviceIndex < ENUM_TO_UINT(EInputDevices::TOTAL));

		return DetectedInput;
	}

	bool GetDown(EInputKeys Key)
	{
		if (Key == EInputKeys::TOTAL)
			return false;

		bool DetectedInput = false;
		int DeviceIndex = 0;
		do
		{
			DetectedInput = Devices[DeviceIndex].Keys[ENUM_TO_UINT(Key)].GetDown();
			++DeviceIndex;
		} while (!DetectedInput && DeviceIndex < ENUM_TO_UINT(EInputDevices::TOTAL));

		return DetectedInput;
	}
};

