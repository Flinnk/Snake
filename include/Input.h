#pragma once

#define ENUM_TO_UINT(x) static_cast<unsigned int>(x)

enum class EInputKeys : unsigned int
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
	void Update();
	bool Get(EInputKeys Key);
	bool GetUp(EInputKeys Key);
	bool GetDown(EInputKeys Key);
	
	SInputDevice Devices[2];
};

extern CInput InputManager;

