@echo off
cls
del OGL.exe
del OGL.obj
del OGL.res
cls

cl.exe /Fo:..\bin\OGL.obj /c /EHsc /I "C:\\glew-2.1.0\\include" ..\src\core\OGL.cpp
rc.exe /Fo ..\bin\OGL.res ..\res\icon\OGL.rc

link.exe ..\bin\OGL.obj ..\bin\OGL.res user32.lib gdi32.lib /LIBPATH:"C:\\glew-2.1.0\\lib\\Release\\x64" /OUT:..\bin\OGL.exe /SUBSYSTEM:WINDOWS
..\bin\OGL.exe
