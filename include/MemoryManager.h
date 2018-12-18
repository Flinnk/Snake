#pragma once
#include <LinearAllocator.h>

#define ENGINE_MEMORY_SIZE 1024*1024*5
#define FRAME_MEMORY_SIZE 1024*1024*50
#define SCENE_MEMORY_SIZE 1024*1024*50

class CMemoryManager
{
public:
	bool Initialize();
	void* AllocateFrameMemory(size_t Size);
	void FreeFrameMemory();
	void* AllocateEngineMemory(size_t Size);
	void* AllocateSceneMemory(size_t Size);
	void FreeSceneMemory();

	CLinearAllocator* const GetSceneMemoryAllocator()
	{
		return &SceneMemory;
	}
	void Release();

private:
	CLinearAllocator EngineMemory;
	CLinearAllocator FrameMemory;
	CLinearAllocator SceneMemory;
};

extern CMemoryManager MemoryManager;