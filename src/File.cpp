#include <File.h>
#include <Windows.h>

bool CFile::Open(const char* Path, EFileOpenMode OpenMode, CFile& File)
{
	File.Close();

	DWORD AccessMode = 0;
	AccessMode |= (OpenMode == EFileOpenMode::READ ? GENERIC_READ : 0x00000000L);
	AccessMode |= (OpenMode == EFileOpenMode::WRITE ? GENERIC_WRITE : 0x00000000L);

	DWORD CreationDisposition = 0;
	switch (OpenMode)
	{
	case EFileOpenMode::READ:
	{
		CreationDisposition = OPEN_EXISTING;
		break;
	}
	case EFileOpenMode::WRITE:
	{
		CreationDisposition = OPEN_ALWAYS;
		break;
	}
	case EFileOpenMode::READ_WRITE:
	{
		CreationDisposition = OPEN_ALWAYS;
		break;
	}
	}

	HANDLE Handle = CreateFile(Path, AccessMode, 0, 0, CreationDisposition, 0, 0);
	if (Handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	File.PlatformHandle = Handle;
	return true;
}

bool CFile::GetFileSize(int64_t& FileSize)
{
	FileSize = 0;
	LARGE_INTEGER Size = {};
	if (GetFileSizeEx(PlatformHandle, &Size))
	{
		FileSize = Size.QuadPart;
		return true;
	}

	return false;
}

bool CFile::ReadAll(void* Buffer, uint64_t* OutBytesRead)
{
	int64_t Size;
	DWORD BytesRead = 0;
	if (!GetFileSize(Size))
		return false;
	bool Result  = ReadFile(PlatformHandle, Buffer, Size, &BytesRead, 0);
	*OutBytesRead = BytesRead;
	return Result;
}

bool CFile::Read(void* Buffer, int64_t Size, uint64_t* OutBytesRead)
{
	DWORD BytesRead = 0;
	bool Result =  ReadFile(PlatformHandle, Buffer, Size, &BytesRead, 0);
	*OutBytesRead = BytesRead;
	return Result;
}

bool CFile::SetPointerPosition(EPointerPosition Position, int64_t Distance)
{
	DWORD Result = SetFilePointer(PlatformHandle, Distance, NULL, ((int)Position));
	if (Result == INVALID_SET_FILE_POINTER)
		return false;
	return true;
}

void CFile::Close()
{
	CloseHandle(PlatformHandle);
}

