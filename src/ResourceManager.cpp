#include <ResourceManager.h>
#include <Renderer.h>
#include <Engine.h>

void CResourceManager::LoadFont(const char* Path, int Size, int BitFontWidth, int BitFontHeight)
{
	if (Fonts.find(Path) == Fonts.end())
	{
		Fonts[Path] = GetEngine()->Renderer.LoadFont(Path, Size, BitFontWidth, BitFontHeight);
	}
}

CFont* CResourceManager::LoadAndRetrieveFont(const char* Path, int Size, int BitFontWidth, int BitFontHeight)
{
	CFont* Font = nullptr;
	if (Fonts.find(Path) == Fonts.end())
	{
		CFont LoadedFont = GetEngine()->Renderer.LoadFont(Path, Size, BitFontWidth, BitFontHeight);
		Fonts[Path] = LoadedFont;
		Font = &Fonts[Path];
	}

	return Font;
}

CFont* CResourceManager::RetrieveFont(const char* Path)
{
	CFont* Font = nullptr;
	if (Fonts.find(Path) != Fonts.end())
		return &Fonts[Path];

	return Font;
}

void CResourceManager::ReleaseFont(const char* Path)
{
	if (Fonts.find(Path) != Fonts.end())
	{
		CFont* Font = &Fonts[Path];
		Font->Release();
		Fonts.erase(Path);
	}
}

void CResourceManager::LoadAudioClip(const char* Path, bool Looped)
{
	if (Audios.find(Path) == Audios.end())
	{
		CAudioClip Clip;
		GetEngine()->AudioManager.LoadAudio(Path, &Clip, Looped);
		Audios[Path] = Clip;
	}
}

CAudioClip* CResourceManager::LoadAndRetrieveAudioClip(const char* Path, bool Looped)
{
	CAudioClip* Clip = nullptr;
	if (Audios.find(Path) == Audios.end())
	{
		CAudioClip LoadedClip;
		GetEngine()->AudioManager.LoadAudio(Path, &LoadedClip, Looped);
		Audios[Path] = LoadedClip;
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