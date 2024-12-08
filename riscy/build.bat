@echo off

setlocal

cd %~dp0

if not exist build mkdir build
cd build

if "%Platform%" neq "x64" (
	echo ERROR: Platform is not "x64" - please run this from the MSVC x64 native tools command prompt.
	goto end
)

set "compile_options=/nologo /W3 /Od /Zo /Z7 /RTC1 /MTd"
set "link_options=/incremental:no /opt:ref /subsystem:console /DEBUG:FULL libucrtd.lib libvcruntimed.lib"

cl %compile_options% ..\src\main.c /link %link_options% /pdb:riscy.pdb /out:riscy.exe

endlocal
