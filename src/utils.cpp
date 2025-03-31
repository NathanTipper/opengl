#include "utils.h"
#include <cstdio>

#ifdef PLATFORM_WIN32
#include <windows.h>
Win32ReadFileResult win32ReadFile(char* filename)
{
    Win32ReadFileResult Result = {0};
    HANDLE fHandle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(fHandle == INVALID_HANDLE_VALUE)
    {
	printf("Could not get handle to file");
	return Result;
    }

    LARGE_INTEGER FileSize;
    if(GetFileSizeEx(fHandle, &FileSize))
    {
        unsigned int FileSize32 = (unsigned int)FileSize.QuadPart;
	Result.contents = VirtualAlloc(0, FileSize32, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	DWORD readBytes;
	if(ReadFile(fHandle, Result.contents, FileSize32, &readBytes, 0) && FileSize32 == readBytes)
	{
	    Result.filesize = FileSize32;
	}
	else
	{
	    VirtualFree(Result.contents, FileSize32, MEM_RELEASE);
	    Result.filesize = 0;
	}
    }

    CloseHandle(fHandle);

    return Result;
}

void win32Free(void *vp, unsigned int size)
{
    VirtualFree(&vp, size, MEM_RELEASE);
}
#endif
