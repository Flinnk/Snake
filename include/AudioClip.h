#pragma once
#include <xaudio2.h>

class CAudioClip
{
public:
	CAudioClip() :SourceVoice(nullptr), DataBuffer(nullptr) {}
	void Initialize(IXAudio2SourceVoice* SourceVoice, BYTE * DataBuffer, XAUDIO2_BUFFER ContentBuffer);
	void Play();
	void Stop();
	void SetVolume(float Volume);
	void Release();
private:
	IXAudio2SourceVoice* SourceVoice;
	BYTE * DataBuffer;
	XAUDIO2_BUFFER Buffer;
};