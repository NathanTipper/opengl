#include "utils.h"
#include <cstdio>
#include <windows.h>
// TODO: ^^^^ @ntipper FIND OUT HOW TO GET RID OF WINDOWS.H ^^^^

bool win32ReadFile(char* filename, char* buffer, int bytesToRead)
{
    HANDLE fHandle = CreateFileA(filename, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(!fHandle)
    {
	return false;
    }

    printf("Reading file %s", filename);
    LPDWORD readBytes;
    void* vbuffer[512];
    if(ReadFile(fHandle, vbuffer, bytesToRead, readBytes, 0))
    {
	printf("\nBytes to read: %d\nBytes read: %lu\nBuffer: %s\n", bytesToRead, *readBytes, buffer);
    }
    else
    {
	DWORD error = GetLastError();
	printf("Could not read file.. error %lu occured", error);
	return false;
    }

    return true;
}
