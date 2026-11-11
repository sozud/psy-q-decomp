#!/bin/bash
# macOS replacement for dosemu_wrapper.sh / aspsx_wrapper.sh: runs the DOS
# PSY-Q tools under daze (https://github.com/mkst/daze), expected at build/daze.
#   daze_wrapper.sh compile <in.c> <out.obj> <flags> <version>
#   daze_wrapper.sh aspsx   <in.d> <out.obj> <aspsx flags> <toolchain folder>
set -e
[ "$#" -eq 5 ] || { echo "Usage: $0 compile|aspsx input output flags version" >&2; exit 1; }
mode=$1
in=$(echo "$2" | tr '/' '\\')
out=$(echo "$3" | tr '/' '\\')
flags=$4
ver=$5
DAZE=${DAZE:-./build/daze}
# per-invocation temp dir so parallel ninja jobs don't share CCPSX's PQn files
mkdir -p TMP
tmp=$(mktemp -d TMP/XXXXXX)
trap 'rm -rf -- "$tmp"' EXIT
env_args=(
  --no-tmp
  --env "TMP=C:\\TMP\\${tmp#TMP/}"
  --env "TEMP=C:\\TMP\\${tmp#TMP/}"
  --env "COMPILER_PATH=C:\\build\\$ver"
  --env 'C_INCLUDE_PATH=C:\include'
  --env "PSYQ_PATH=C:\\build\\$ver"
)
rm -f "$3"
case "$mode" in
  compile)
    "$DAZE" "${env_args[@]}" "build/$ver/CCPSX.EXE" $flags -O2 -g0 -G0 -funsigned-char -c -I./src/snd "$in" "-o$out" ;;
  aspsx)
    # $flags = assembler flags (e.g. -G0; empty for 3.0's ASPSX 1.07)
    "$DAZE" "${env_args[@]}" "build/$ver/ASPSX.EXE" $flags "$in" "-o$out" ;;
  *) echo "unknown mode $mode" >&2; exit 1 ;;
esac
# DOS tools don't always return a failing exit code
test -f "$3"
