#include <LinearAllocator.h>
#include <cstdlib>
#include <cstring>
bool CLinearAllocator::Initialize(size_t Size)
{
	MemoryChunckLocation = malloc(Size);
	memset(MemoryChunckLocation, 0, Size);
	Offset = 0;
	ChunckSize = Size;
	return MemoryChunckLocation != nullptr;
}

void* CLinearAllocator::Allocate(size_t Size)
{
	const size_t CurrentAddress = (size_t)MemoryChunckLocation + Offset;
	if (Size + Offset > ChunckSize)
		return nullptr;

	size_t NextAddress = CurrentAddress;
	Offset += Size;
	return (void*)NextAddress;
}

void CLinearAllocator::Free()
{
	Offset = 0;
	memset(MemoryChunckLocation, 0, ChunckSize);
}

void CLinearAllocator::Release()
{
	free(MemoryChunckLocation);
	MemoryChunckLocation = nullptr;
	Offset = 0;
	ChunckSize = 0;
}