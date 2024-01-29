set COMPILER_PATH=G:/build
set C_INCLUDE_PATH=G:/psy-q/3.5/PSX/include
set PSYQ_PATH=G:/build
set arg1=%1
set arg2=%2
shift
shift
dir build
G:\build\ccpsx.exe -v -O2 -g0 -G0 -funsigned-char -c -I./src/snd %arg1% -o%arg2%