#include <ResourceManager.h>
#include <Renderer.h>
#include <Engine.h>

void CResourceManager::LoadFont(const char* Path, int Size, int BitFontWidth, int BitFontHeight)
{
	if (Resources.find(Path) == Resources.end())
	{
		Resources[Path] = GetEngine()->Renderer.LoadFont(Path, Size, BitFontWidth, BitFontHeight);
	}
}

CFont* CResourceManager::LoadAndRetrieveFont(const char* Path, int Size, int BitFontWidth, int BitFontHeight)
{
	CFont* Font = nullptr;
	if (Resources.find(Path) == Resources.end())
	{
		CFont LoadedFont = GetEngine()->Renderer.LoadFont(Path, Size, BitFontWidth, BitFontHeight);
		Resources[Path] = LoadedFont;
		Font = &LoadedFont;
	}

	return Font;
}

CFont* CResourceManager::RetrieveFont(const char* Path)
{
	CFont* Font = nullptr;
	if (Resources.find(Path) != Resources.end())
		return &Resources[Path];

	return Font;
}

void CResourceManager::ReleaseFont(const char* Path)
{
	if (Resources.find(Path) != Resources.end())
	{
		CFont* Font = &Resources[Path];
		Font->Release();
		Resources.erase(Path);
	}
}