#include <MemoryManager.h>

CMemoryManager MemoryManager;

bool CMemoryManager::Initialize()
{
	if (!EngineMemory.Initialize(ENGINE_MEMORY_SIZE))
		return false;

	if (!FrameMemory.Initialize(FRAME_MEMORY_SIZE))
		return false;

	if (!SceneMemory.Initialize(SCENE_MEMORY_SIZE))
		return false;

	return true;
}

void* CMemoryManager::AllocateFrameMemory(size_t Size)
{
	return FrameMemory.Allocate(Size);
}

void CMemoryManager::FreeFrameMemory()
{
	FrameMemory.Free();
}

void* CMemoryManager::AllocateEngineMemory(size_t Size)
{
	return EngineMemory.Allocate(Size);
}

void* CMemoryManager::AllocateSceneMemory(size_t Size)
{
	return SceneMemory.Allocate(Size);
}

void CMemoryManager::FreeSceneMemory()
{
	SceneMemory.Free();
}


void CMemoryManager::Release()
{
	FrameMemory.Release();
	SceneMemory.Release();
	EngineMemory.Release();
}