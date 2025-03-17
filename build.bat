@echo off

if exist .\build\ (
    rmdir /s /q .\build
)

mkdir .\build

pushd .\build
cl /I ..\include\ /Od ..\src\main.cpp ..\src\glad.c /link glfw3_mt.lib opengl32.lib user32.lib gdi32.lib Shell32.lib /LIBPATH:..\lib
del *.obj
popd
