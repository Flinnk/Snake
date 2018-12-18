#pragma once

class CLinearAllocator
{

public:

	CLinearAllocator() {}
	bool Initialize(size_t Size);

	void* Allocate(size_t Size);

	void Free();

	void Release();
private:
	void* MemoryChunckLocation = nullptr;
	size_t Offset = 0;
	size_t ChunckSize;
};