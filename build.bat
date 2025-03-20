@echo off

if exist .\build\ (
    rmdir /s /q .\build
)

mkdir .\build

pushd .\build
cl /I ..\include\ /Od /DPLATFORM_WIN32=1 ..\src\main.cpp ..\src\glad.c ..\src\utils.cpp ..\src\shader.cpp /link /DEBUG glfw3_mt.lib opengl32.lib user32.lib gdi32.lib Shell32.lib Kernel32.lib /LIBPATH:..\lib
del *.obj
popd
