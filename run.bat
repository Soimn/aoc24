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

if not exist build mkdir build
cd build


set "common_compile_options= /nologo /W3"
set "common_link_options= /incremental:no /opt:ref /subsystem:console"

set "compile_options=%common_compile_options% /Od /Zo /Z7 /RTC1 /MTd"
set "link_options=%common_link_options% /DEBUG:FULL libucrtd.lib libvcruntimed.lib"

if "%2"=="" goto invalid_arguments
if not exist ../%2 goto invalid_arguments

if "%3"=="release" (
  set "compile_options=%common_compile_options% /O2 /arch:AVX2 /DDISABLE_ASSERT"
) else if "%3" neq "" goto invalid_arguments

cl %compile_options% ..\main.c /link %link_options% /pdb:day%1.pdb /out:day%1.exe && day%1.exe ..\%2

goto end

:invalid_arguments
echo Invalid arguments^. Usage: run day input_file [release]
goto end

:end
endlocal
