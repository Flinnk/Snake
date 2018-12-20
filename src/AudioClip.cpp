#include <AudioClip.h>

void CAudioClip::Initialize(IXAudio2SourceVoice* SourceVoiceParameter, BYTE * DataBufferParameter, XAUDIO2_BUFFER ContentBuffer)
{
	SourceVoice = SourceVoiceParameter;
	DataBuffer = DataBufferParameter;
	Buffer = ContentBuffer;
}

void CAudioClip::Play()
{
	Stop();
	if (SourceVoice)
	{
		SourceVoice->SubmitSourceBuffer(&Buffer);
		SourceVoice->Start(0, 0);
	}
}

void CAudioClip::Stop()
{
	if (SourceVoice)
		SourceVoice->Stop(0, 0);
}
void CAudioClip::SetVolume(float Volume)
{
	if (SourceVoice)
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