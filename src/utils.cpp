#include "utils.h"
#include <cstdio>
#include "nmath.h"

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

int stringLength(char *s)
{
    int Result = 0;

    if(!s)
    {
	return Result;
    }

    char c;
    int i = 0;
    while((c = s[i++]))
    {
	++Result;
    }

    return Result;
}

bool stringEqual(char *a, char *b)
{
    bool Result = false;
    if(!a || !b)
    {
	return Result;
    }

    char* iter_a = a;
    char* iter_b = b;

    while(*iter_a != '\0' && *iter_b != '\0')
    {
	if(*iter_a != *iter_b)
	{
	    break;
	}

	++iter_a;
	++iter_b;
    }

    if(*iter_a == '\0' && *iter_b == '\0')
    {
	Result = true;
    }

    return Result;
}

bool stringToFloat(char* string, int length, float* outFloat)
{
    bool Result = false;

    if(!string || length < 1)
    {
	return Result; 
    }

    *outFloat = 0.f;
    int div = 0;
    int exp = 0;
    char c;
    for(int i = length - 1; i >= 0; --i)
    {
	c = string[i];
	if(c == '.')
	{
	    div = power(10, (length-1) - i);
	    continue;
	}

	if(c >= 48 && c <= 57) // isdigit
	{
	    *outFloat += (c - 48) * power(10, exp);
	    ++exp;
	}
	else if(c == '-')
	{
	    *outFloat *= -1.f;
	}
	else
	{
	    return Result;
	}
    }

    if(div > 0)
    {
	*outFloat /= div;
    }

    Result = true;
    return Result;
}

void intToString(int a, char *s, int length)
{
    if(!s)
    {
	return;
    }

    int cp = a;
    int numDigits = 0;
    while(cp != 0)
    {
	cp /= 10;
	++numDigits;
    }

    if(numDigits >= length - 1)
    {
	printf("INT COULD NOT BE STORED IN BUFFER");
	return;
    }

    for(int i = numDigits - 1; i >= 0; --i)
    {
	s[i] = (a % 10) + 48;
	a /= 10;
    }
    s[numDigits] = '\0';
}

int findLastInString(char *s, char f)
{
    int Result = -1;

    if(!s)
    {
	return Result;
    }

    int currentIndex = 0;
    char c;
    while((c = s[currentIndex]))
    {
	if(c == f)
	{
	    Result = currentIndex;
	}

	++currentIndex;
    }

    return Result;
}

bool substring(char *dest, int dlength, char* source, int si, int ei)
{
    int slength = stringLength(source);
    if(!dest || !slength)
    {
	return false;
    }

    if(ei < 0)
    {
	ei = slength - 1;
    }

    if(ei > slength || (ei - si) >= dlength - 1)
    {
	return false;
    }

    int dindex = 0;
    for(int i = si; i <= ei; ++i)
    {
	dest[dindex++] = source[i];
    }
    dest[dindex] = '\0';

    return true;
}
