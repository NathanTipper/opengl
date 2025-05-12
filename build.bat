@echo off

pushd .\build
del *.exe
del *.pdb
del *.ilk
cl /I ..\include\ /Od /DPLATFORM_WIN32=1 /Zi /EHsc ..\src\main.cpp ..\src\glad.c ..\src\utils.cpp ..\src\shader.cpp ..\src\nmath.cpp /link /DEBUG glfw3_mt.lib opengl32.lib user32.lib gdi32.lib Shell32.lib Kernel32.lib assimp.lib /LIBPATH:..\lib
del *.obj
popd
