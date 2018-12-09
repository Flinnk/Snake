#pragma once
#include <map>
#include <AudioClip.h>

class CResourceManager 
{
public:
	CResourceManager(){}

	void LoadAudioClip(const char* Path, bool Looped);
	CAudioClip* LoadAndRetrieveAudioClip(const char* Path, bool Looped);
	CAudioClip* RetrieveAudioClip(const char* Path);
	void ReleaseAudioClip(const char* Path);
private:
	std::map<std::string, CAudioClip> Audios;

};

extern CResourceManager ResourceManager;
