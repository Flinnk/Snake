#pragma once
#include <cstdint>

enum class EFileOpenMode
{
	READ,
	WRITE,
	READ_WRITE
};

enum class EPointerPosition
{
	BEGIN,
	CURRENT,
	END
};

class CFile
{
public:
	static bool Open(const char* Path, EFileOpenMode OpenMode,CFile& File);

	bool GetFileSize(int64_t& Size);
	bool ReadAll(void* Buffer, uint64_t* BytesRead);
	bool Read(void* Buffer, int64_t Size, uint64_t* BytesRead);
	bool SetPointerPosition(EPointerPosition Position, int64_t Distance);

	void Close();
private:
	void* PlatformHandle = nullptr;
};