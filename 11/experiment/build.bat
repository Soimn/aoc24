@echo off

setlocal

cd %~dp0

if "%Platform%" neq "x64" (
	echo ERROR: Platform is not "x64" - please run this from the MSVC x64 native tools command prompt.
	goto end
)

if not exist build mkdir build
cd build


set "compile_options= /nologo /W3 /O2 /arch:AVX2 /DDISABLE_ASSERT"
set "link_options= /incremental:no /opt:ref /subsystem:console"

cl %compile_options% ..\hyperneutrino_main.c /link %link_options% /pdb:hyperneutrino.pdb /out:hyperneutrino.exe
cl %compile_options% ..\fang_main.c /link %link_options% /pdb:feng.pdb /out:feng.exe

goto end

:invalid_arguments
echo Invalid arguments^. Usage: run day input_file [release]
goto end

:end
endlocal
