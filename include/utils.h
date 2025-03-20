#ifndef UTILS_H
#define UTILS_H

#ifdef PLATFORM_WIN32
struct Win32ReadFileResult
{
  long int filesize;
  void* contents;
};

Win32ReadFileResult win32ReadFile(char* filename);
#endif
#endif

