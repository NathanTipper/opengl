#include <cstdio>
#include <windows.h>
// TODO: ^^^^ @ntipper FIND OUT HOW TO GET RID OF WINDOWS.H ^^^^

bool win32ReadFile(char* filename, void*& buffer)
{
    bool Result;
    HANDLE fHandle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(fHandle == INVALID_HANDLE_VALUE)
    {
	printf("Could not get handle to file");
	return false;
    }

    LARGE_INTEGER FileSize;
    if(GetFileSizeEx(fHandle, &FileSize))
    {
        unsigned int FileSize32 = (unsigned int)FileSize.QuadPart;
	printf("File size: %ul", FileSize32);
	buffer = VirtualAlloc(0, FileSize32, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	DWORD readBytes;
	if(ReadFile(fHandle, buffer, FileSize32, &readBytes, 0))
	{
	    printf("Successfully read file... buffer: %s", (char*)buffer);
	    Result = true;
	}
	else
	{
	    Result = false;
	}
    }

    CloseHandle(fHandle);

    return Result;
}
