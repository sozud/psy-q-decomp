@echo off
set arg1=%1
set arg2=%2
set arg3=%3
set arg4=%4
shift
shift
shift
shift
set COMPILER_PATH=%CD%build\%arg4%
set C_INCLUDE_PATH=%CD%include
set PSYQ_PATH=%CD%build\%arg4%
@echo on
%CD%build\%arg4%\ccpsx.exe %arg3% -O2 -g0 -G0 -funsigned-char -c -I./src/snd %arg1% -o%arg2%