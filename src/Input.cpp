#include <Input.h>
#include <Windows.h>
#include <Xinput.h>
CInput InputManager;

bool CInput::Get(EInputKeys Key)
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

bool CInput::GetUp(EInputKeys Key)
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

bool CInput::GetDown(EInputKeys Key)
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

void CInput::Update()
{
	for (int i = 0; i < ENUM_TO_UINT(EInputDevices::TOTAL); ++i)
	{
		SInputDevice&  Device = InputManager.Devices[i];
		for (int j = 0; j < ENUM_TO_UINT(EInputKeys::TOTAL); ++j)
		{
			Device.Keys[j].PreviousState = Device.Keys[j].CurrentState;
		}
	}

	InputManager.Devices[ENUM_TO_UINT(EInputDevices::KEYBOARD)].Keys[ENUM_TO_UINT(EInputKeys::UP)].CurrentState = (GetAsyncKeyState(VK_UP) & 0x8000) != 0 || (GetAsyncKeyState('W') & 0x8000) != 0;
	InputManager.Devices[ENUM_TO_UINT(EInputDevices::KEYBOARD)].Keys[ENUM_TO_UINT(EInputKeys::DOWN)].CurrentState = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0 || (GetAsyncKeyState('S') & 0x8000) != 0;
	InputManager.Devices[ENUM_TO_UINT(EInputDevices::KEYBOARD)].Keys[ENUM_TO_UINT(EInputKeys::LEFT)].CurrentState = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0 || (GetAsyncKeyState('A') & 0x8000) != 0;
	InputManager.Devices[ENUM_TO_UINT(EInputDevices::KEYBOARD)].Keys[ENUM_TO_UINT(EInputKeys::RIGHT)].CurrentState = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0 || (GetAsyncKeyState('D') & 0x8000) != 0;
	InputManager.Devices[ENUM_TO_UINT(EInputDevices::KEYBOARD)].Keys[ENUM_TO_UINT(EInputKeys::START)].CurrentState = (GetAsyncKeyState(VK_RETURN) & 0x8000) != 0;

	DWORD Result;
	XINPUT_STATE State;
	ZeroMemory(&State, sizeof(XINPUT_STATE));

	// Simply get the state of the controller from XInput.
	Result = XInputGetState(0, &State);

	if (Result == ERROR_SUCCESS)
	{
		// Controller is connected 
		InputManager.Devices[ENUM_TO_UINT(EInputDevices::GAMEPAD)].Keys[ENUM_TO_UINT(EInputKeys::UP)].CurrentState = State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
		InputManager.Devices[ENUM_TO_UINT(EInputDevices::GAMEPAD)].Keys[ENUM_TO_UINT(EInputKeys::DOWN)].CurrentState = State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
		InputManager.Devices[ENUM_TO_UINT(EInputDevices::GAMEPAD)].Keys[ENUM_TO_UINT(EInputKeys::LEFT)].CurrentState = State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
		InputManager.Devices[ENUM_TO_UINT(EInputDevices::GAMEPAD)].Keys[ENUM_TO_UINT(EInputKeys::RIGHT)].CurrentState = State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
		InputManager.Devices[ENUM_TO_UINT(EInputDevices::GAMEPAD)].Keys[ENUM_TO_UINT(EInputKeys::START)].CurrentState = State.Gamepad.wButtons & XINPUT_GAMEPAD_START;

	}
	else
	{
		// Controller is not connected 
	}
}
