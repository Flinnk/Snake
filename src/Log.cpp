#include <Log.h>
#include <Windows.h>

void ShowSystemErrorMessage(const char* Message)
{
	MessageBox(NULL, Message, "Error", MB_OK | MB_ICONERROR);
}

void DebugLog(const char* Message)
{
	OutputDebugString(TEXT(Message));
}