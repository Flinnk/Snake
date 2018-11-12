#include <Audio.h>

HRESULT CAudioManager::FindChunk(HANDLE File, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(File, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

	while (hr == S_OK)
	{
		DWORD dwRead;
		if (0 == ReadFile(File, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		if (0 == ReadFile(File, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		switch (dwChunkType)
		{
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (0 == ReadFile(File, &dwFileType, sizeof(DWORD), &dwRead, NULL))
				hr = HRESULT_FROM_WIN32(GetLastError());
			break;

		default:
			if (INVALID_SET_FILE_POINTER == SetFilePointer(File, dwChunkDataSize, NULL, FILE_CURRENT))
				return HRESULT_FROM_WIN32(GetLastError());
		}

		dwOffset += sizeof(DWORD) * 2;

		if (dwChunkType == fourcc)
		{
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize) return S_FALSE;

	}

	return S_OK;

}


HRESULT CAudioManager::ReadChunkData(HANDLE File, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(File, bufferoffset, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());
	DWORD dwRead;
	if (0 == ReadFile(File, buffer, buffersize, &dwRead, NULL))
		hr = HRESULT_FROM_WIN32(GetLastError());
	return hr;
}

bool CAudioManager::LoadAudio(const char* FilePath, CAudioClip* Clip, bool Loop)
{
	HANDLE File = CreateFile(
		FilePath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (INVALID_HANDLE_VALUE == File)
		return false;

	if (INVALID_SET_FILE_POINTER == SetFilePointer(File, 0, NULL, FILE_BEGIN))
		return false;

	WAVEFORMATEXTENSIBLE WAVEFORMAT = { 0 };
	XAUDIO2_BUFFER Buffer = { 0 };

	DWORD ChunkSize;
	DWORD ChunkPosition;
	//check the file type, should be fourccWAVE or 'XWMA'
	FindChunk(File, fourccRIFF, ChunkSize, ChunkPosition);
	DWORD filetype;
	ReadChunkData(File, &filetype, sizeof(DWORD), ChunkPosition);
	if (filetype != fourccWAVE)
		return false;

	FindChunk(File, fourccFMT, ChunkSize, ChunkPosition);
	ReadChunkData(File, &WAVEFORMAT, ChunkSize, ChunkPosition);

	//fill out the audio data buffer with the contents of the fourccDATA chunk
	FindChunk(File, fourccDATA, ChunkSize, ChunkPosition);
	BYTE * DataBuffer = new BYTE[ChunkSize];
	ReadChunkData(File, DataBuffer, ChunkSize, ChunkPosition);

	Buffer.AudioBytes = ChunkSize;  //buffer containing audio data
	Buffer.pAudioData = DataBuffer;  //size of the audio buffer in bytes
	Buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
	Buffer.LoopCount = Loop ? XAUDIO2_LOOP_INFINITE : 0;
	IXAudio2SourceVoice* SourceVoice;
	HRESULT hr;
	if (FAILED(hr = XAudio2->CreateSourceVoice(&SourceVoice, (WAVEFORMATEX*)&WAVEFORMAT)))
		return false;

	if (FAILED(hr = SourceVoice->SubmitSourceBuffer(&Buffer)))
		return false;

	Clip->Initialize(SourceVoice, DataBuffer);
	return true;
}

bool CAudioManager::Initialize()
{
	HRESULT Result = XAudio2Create(&XAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(Result))
		return false;

	if (FAILED(Result = XAudio2->CreateMasteringVoice(&MasterVoice)))
		return false;

	return true;
}

void CAudioManager::Release()
{
	XAudio2->Release();
}

void CAudioManager::StopEngine()
{
	XAudio2->StopEngine();
}

void CAudioManager::StartEngine()
{
	XAudio2->StartEngine();
}

void CAudioClip::Initialize(IXAudio2SourceVoice* SourceVoiceParameter, BYTE * DataBufferParameter)
{
	SourceVoice = SourceVoiceParameter;
	DataBuffer = DataBufferParameter;
}

void CAudioClip::Play()
{
	SourceVoice->Start(0, 0);
}

void CAudioClip::Stop()
{
	SourceVoice->Stop();
}
void CAudioClip::SetVolume(float Volume)
{
	SourceVoice->SetVolume(Volume);
}

void CAudioClip::Release()
{
	if (SourceVoice)
	{
		SourceVoice->Stop();
		SourceVoice->DestroyVoice();
	}
	if (DataBuffer)
		delete DataBuffer;
}