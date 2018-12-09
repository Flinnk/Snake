#pragma once
#include <Renderer.h>
#include <Input.h>
#include <AudioManager.h>
#include <ResourceManager.h>


struct CEngine
{
public:
	CEngine() {};

	void RequestExit()
	{
		bClose = true;
	}
	bool HasRequestExit()
	{
		return bClose;
	}
private:
	bool bClose=false;
};


extern CEngine* GetEngine();
