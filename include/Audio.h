#pragma once
#include <xaudio2.h>

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

class CAudioClip
{
public:
	CAudioClip() :SourceVoice(nullptr), DataBuffer(nullptr) {}
	void Initialize(IXAudio2SourceVoice* SourceVoice, BYTE * DataBuffer);
	void Play();
	void Stop();
	void SetVolume(float Volume);
	void Release();
private:
	IXAudio2SourceVoice* SourceVoice;
	BYTE * DataBuffer;
};

class CAudioManager
{
public:
	CAudioManager() :XAudio2(nullptr), MasterVoice(nullptr) {}
	bool Initialize();
	bool LoadAudio(const char* FilePath, CAudioClip* Clip, bool Loop);
	void Release();
	void StopEngine();
	void StartEngine();
private:

	HRESULT FindChunk(HANDLE File, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition);
	HRESULT ReadChunkData(HANDLE File, void * buffer, DWORD buffersize, DWORD bufferoffset);

	IXAudio2* XAudio2;
	IXAudio2MasteringVoice* MasterVoice;
};