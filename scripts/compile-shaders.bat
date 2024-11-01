@echo off
rem Compile shaders (Windows)
rem Compile shaders (using glslc) from Input into Output folder
rem made by vertoker

cd ../..
set PROJECT_PATH=%cd%\

rem Variables
set RESOURCES_INPUT=%cd%\src\shaders\
set RESOURCES_OUTPUT=%cd%\resources\shaders\

@echo on

md %RESOURCES_OUTPUT%

glslc %RESOURCES_INPUT%simple.vert -o %RESOURCES_OUTPUT%simple.vert.spv
glslc %RESOURCES_INPUT%simple.frag -o %RESOURCES_OUTPUT%simple.frag.spv

pause
