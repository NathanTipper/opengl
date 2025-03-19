#ifndef UTILS_H
#define UTILS_H

struct Win32ReadFileResult
{
  long int filesize;
  void* contents;
};

Win32ReadFileResult win32ReadFile(char* filename);

#endif
