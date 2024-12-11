@echo off

setlocal

cd %~dp0

if "%Platform%" neq "x64" (
	echo ERROR: Platform is not "x64" - please run this from the MSVC x64 native tools command prompt.
	goto end
)

if "%1"=="" goto invalid_arguments
if not exist %1 goto invalid_arguments

cd %1

if "%2"=="" goto invalid_arguments
if not exist %2 goto invalid_arguments
if "%3" neq "" goto invalid_arguments

call ..\riscy\build.bat && ..\riscy\build\riscy.exe main.rv %2

goto end

:invalid_arguments
echo Invalid arguments^. Usage: run day input_file
goto end

:end
endlocal
