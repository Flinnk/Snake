#include <PlatformTime.h>
#include <Windows.h>

static LARGE_INTEGER Frequency;
static LARGE_INTEGER InitialCounter;

void InitializeTime()
{
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&InitialCounter);
}

float GetEllapsedMilliseconds()
{
	LARGE_INTEGER CurrentCounter;
	QueryPerformanceCounter(&CurrentCounter);

	LONGLONG ElapsedCounter = CurrentCounter.QuadPart - InitialCounter.QuadPart;
	return (float(ElapsedCounter) / float(Frequency.QuadPart));
}