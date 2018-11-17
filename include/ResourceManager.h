#pragma once
#include <map>
class CFont;
class CResourceManager 
{
public:
	CResourceManager(){}

	void LoadFont(const char* Path, int Size, int BitFontWidth, int BitFontHeight);
	CFont* LoadAndRetrieveFont(const char* Path, int Size, int BitFontWidth, int BitFontHeight);
	CFont* RetrieveFont(const char* Path);
	void ReleaseFont(const char* Path);
private:
	std::map<std::string, CFont> Resources;
};