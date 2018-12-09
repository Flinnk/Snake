#pragma once
#include <File.h>
#include <AudioClip.h>

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

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

	HRESULT FindChunk(CFile& File, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition);
	HRESULT ReadChunkData(CFile& File, void * buffer, DWORD buffersize, DWORD bufferoffset);

	IXAudio2* XAudio2;
	IXAudio2MasteringVoice* MasterVoice;
};

extern CAudioManager AudioManager;