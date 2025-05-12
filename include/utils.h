#ifndef UTILS_H
#define UTILS_H

#ifdef PLATFORM_WIN32
struct Win32ReadFileResult
{
  long int filesize;
  void* contents;
};

Win32ReadFileResult win32ReadFile(char* filename);
void win32Free(void* vp, unsigned int size);
#endif

int stringLength(char* s);
bool stringEqual(char* a, char* b);
bool stringToFloat(char* string, int length, float* outFloat);
void intToString(int i, char* s, int length);
int findLastInString(char* s, char f);
bool substring(char* dest, int dlength, char* source, int si, int ei);
#endif

