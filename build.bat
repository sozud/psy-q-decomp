set COMPILER_PATH=%CD%build
set C_INCLUDE_PATH=%CD%psy-q/3.5/PSX/include
set PSYQ_PATH=%CD%build
set arg1=%1
set arg2=%2
shift
shift
dir build
%CD%build\ccpsx.exe -v -O2 -g0 -G0 -funsigned-char -c -I./src/snd %arg1% -o%arg2%