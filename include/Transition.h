#pragma once
#include <math.h>

template <class T>
class Transition
{
public:
	Transition(){}
	void Initialize(T InitialValue, T FinalValue, float Duration);
	bool Run(float ElapsedTime,T* CurrentValue);
	void Inverse();
	void Reset();
private:
	T InitialValue;
	T FinalValue;
	float CurrentDuration;
	float Duration;
};


template <class T>
void Transition<T>::Initialize(T InitialValueParam, T FinalValueParam, float DurationParam)
{
	InitialValue = InitialValueParam;
	FinalValue = FinalValueParam;
	Duration = DurationParam;
}

template <class T>
bool Transition<T>::Run(float ElapsedTime, T* CurrentValue)
{
	bool Finished = false;

	if (CurrentDuration < Duration)
	{
		float Delta = CurrentDuration / Duration;
		*CurrentValue = (1 - Delta)*InitialValue + Delta * FinalValue;
		CurrentDuration += ElapsedTime;
		CurrentDuration = fmin(CurrentDuration, Duration);
		Finished = false;
	}
	else
	{
		*CurrentValue = FinalValue;
		Finished = true;
	}

	return Finished;
}

template <class T>
void Transition<T>::Reset()
{
	CurrentDuration = 0.0f;
}

template <class T>
void Transition<T>::Inverse()
{
	T TempValue = InitialValue;
	InitialValue = FinalValue;
	FinalValue = TempValue;
}