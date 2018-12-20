#include <AudioManager.h>
#include <File.h>
#include <LinearAllocator.h>
CAudioManager AudioManager;

HRESULT CAudioManager::FindChunk(CFile& File, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
	HRESULT hr = S_OK;
	if (!File.SetPointerPosition(EPointerPosition::BEGIN,0))
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

	while (hr == S_OK)
	{
		uint64_t dwRead;
		if (!File.Read(&dwChunkType, sizeof(DWORD), &dwRead))
			hr = HRESULT_FROM_WIN32(GetLastError());

		if (!File.Read(&dwChunkDataSize, sizeof(DWORD), &dwRead))
			hr = HRESULT_FROM_WIN32(GetLastError());

		switch (dwChunkType)
		{
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (!File.Read(&dwFileType, sizeof(DWORD), &dwRead))
				hr = HRESULT_FROM_WIN32(GetLastError());
			break;

		default:
			if (!File.SetPointerPosition(EPointerPosition::CURRENT,dwChunkDataSize))
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


HRESULT CAudioManager::ReadChunkData(CFile& File, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
	HRESULT hr = S_OK;
	if (!File.SetPointerPosition(EPointerPosition::BEGIN,bufferoffset))
		return HRESULT_FROM_WIN32(GetLastError());
	uint64_t dwRead;
	if (!File.Read(buffer, buffersize, &dwRead))
		hr = HRESULT_FROM_WIN32(GetLastError());
	return hr;
}

bool CAudioManager::LoadAudio(CLinearAllocator* const Allocator,const char* FilePath, CAudioClip* Clip, bool Loop)
{
	CFile AudioFile;
	if (!CFile::Open(FilePath, EFileOpenMode::READ, AudioFile))
		return false;
	/*HANDLE File = CreateFile(
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
		return false;*/

	WAVEFORMATEXTENSIBLE WAVEFORMAT = { 0 };
	XAUDIO2_BUFFER Buffer = { 0 };

	DWORD ChunkSize;
	DWORD ChunkPosition;
	//check the file type, should be fourccWAVE or 'XWMA'
	FindChunk(AudioFile, fourccRIFF, ChunkSize, ChunkPosition);
	DWORD filetype;
	ReadChunkData(AudioFile, &filetype, sizeof(DWORD), ChunkPosition);
	if (filetype != fourccWAVE)
		return false;

	FindChunk(AudioFile, fourccFMT, ChunkSize, ChunkPosition);
	ReadChunkData(AudioFile, &WAVEFORMAT, ChunkSize, ChunkPosition);

	//fill out the audio data buffer with the contents of the fourccDATA chunk
	FindChunk(AudioFile, fourccDATA, ChunkSize, ChunkPosition);
	BYTE * DataBuffer = (BYTE*)Allocator->Allocate(ChunkSize);
	ReadChunkData(AudioFile, DataBuffer, ChunkSize, ChunkPosition);

	Buffer.AudioBytes = ChunkSize;  //buffer containing audio data
	Buffer.pAudioData = DataBuffer;  //size of the audio buffer in bytes
	Buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
	Buffer.LoopCount = Loop ? XAUDIO2_LOOP_INFINITE : 0;
	IXAudio2SourceVoice* SourceVoice = nullptr;
	HRESULT hr;
	if (FAILED(hr = XAudio2->CreateSourceVoice(&SourceVoice, (WAVEFORMATEX*)&WAVEFORMAT)))
		return false;

	/*if (FAILED(hr = SourceVoice->SubmitSourceBuffer(&Buffer)))
		return false;*/

	Clip->Initialize(SourceVoice, DataBuffer,Buffer);
	AudioFile.Close();
	return true;
}

bool CAudioManager::Initialize()
{
	HRESULT Result = XAudio2Create(&XAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(Result))
		return false;

	if (FAILED(Result = XAudio2->CreateMasteringVoice(&MasterVoice)))
		return false;

	bInitialized = true;
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

