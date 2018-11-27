#pragma once
#include <map>
class CFont;
class CAudioClip;
class CResourceManager 
{
public:
	CResourceManager(){}

	void LoadFont(const char* Path, int Size, int BitFontWidth, int BitFontHeight);
	CFont* LoadAndRetrieveFont(const char* Path, int Size, int BitFontWidth, int BitFontHeight);
	CFont* RetrieveFont(const char* Path);
	void ReleaseFont(const char* Path);

	void LoadAudioClip(const char* Path, bool Looped);
	CAudioClip* LoadAndRetrieveAudioClip(const char* Path, bool Looped);
	CAudioClip* RetrieveAudioClip(const char* Path);
	void ReleaseAudioClip(const char* Path);
private:
	std::map<std::string, CFont> Fonts;
	std::map<std::string, CAudioClip> Audios;

};