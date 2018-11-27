#pragma once
#include <Audio.h>
#include <Renderer.h>
#include <Input.h>
#include <ResourceManager.h>

struct CEngine
{
public:
	CEngine() {};
	
	bool bClose=false;

	CAudioManager AudioManager;
	CRenderer Renderer;
	CInput Input;
	CResourceManager ResourceManager;
};


extern CEngine* GetEngine();