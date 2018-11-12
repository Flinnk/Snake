#pragma once
#include <Audio.h>
#include <Renderer.h>
#include <Input.h>

struct CEngine
{
public:
	CEngine() {};
	
	CAudioManager AudioManager;
	CRenderer Renderer;
	CInput Input;
};


extern CEngine* GetEngine();