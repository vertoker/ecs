@echo off
rem Copy Resources (Windows)
rem Copies Source folder and paste into Debug/Release folders
rem made by vertoker

cd ../..
set PROJECT_PATH=%cd%\

rem Variables
set RESOURCES=%cd%\resources\
set DEBUG_BUILD=%cd%\out\Debug\resources\
set RELEASE_BUILD=%cd%\out\Release\resources\

@echo on

md %RESOURCES%
md %DEBUG_BUILD%
md %RELEASE_BUILD%

@RD /S /Q %DEBUG_BUILD%
@RD /S /Q %RELEASE_BUILD%
xcopy /s %RESOURCES% %DEBUG_BUILD%
xcopy /s %RESOURCES% %RELEASE_BUILD%

pause
