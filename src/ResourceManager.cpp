#include <ResourceManager.h>
#include <Renderer.h>
#include <AudioManager.h>

CResourceManager ResourceManager;

void CResourceManager::LoadAudioClip(const char* Path, bool Looped)
{
	if (Audios.find(Path) == Audios.end())
	{
		CAudioClip Clip;
		AudioManager.LoadAudio(Path, &Clip, Looped);
		Audios[Path] = Clip;
	}
}

CAudioClip* CResourceManager::LoadAndRetrieveAudioClip(const char* Path, bool Looped)
{
	CAudioClip* Clip = nullptr;
	if (Audios.find(Path) == Audios.end())
	{
		CAudioClip LoadedClip;
		AudioManager.LoadAudio(Path, &LoadedClip, Looped);
		Audios[Path] = LoadedClip;
		Clip = &Audios[Path];
	}
	else
	{
		Clip = &Audios[Path];
	}

	return Clip;
}

CAudioClip* CResourceManager::RetrieveAudioClip(const char* Path)
{
	CAudioClip* Clip = nullptr;
	if (Audios.find(Path) != Audios.end())
		return &Audios[Path];

	return Clip;
}

void CResourceManager::ReleaseAudioClip(const char* Path)
{
	if (Audios.find(Path) != Audios.end())
	{
		CAudioClip* Clip = &Audios[Path];
		Clip->Release();
		Audios.erase(Path);
	}
}