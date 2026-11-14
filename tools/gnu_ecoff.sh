#!/bin/bash
set -e
[ "$#" -ge 4 ] || { echo "usage: $0 in out.obj 'cppflags' 'cc1flags' [gccver] [asver]" >&2; exit 2; }
in=$1 out=$2 cppflags=$3 cc1flags=$4 gccver=${5:-cygnus-2.7.2-970404} asver=${6:-2.8-970404}
G=build/gnu/gcc-$gccver
AS=build/gnu/binutils-$asver/as
PY=${PYTHON:-python3}
[ -n "$CC1" ] || [ -x $G/cc1 ] || { echo "$0: $G/cc1 missing (run prep_gnu.sh)" >&2; exit 1; }
[ -x $AS ] || { echo "$0: $AS missing (run prep_gnu.sh)" >&2; exit 1; }
mkdir -p build/gnu/tmp
T=$(mktemp -d build/gnu/tmp/XXXXXX)
if [ -n "$KEEP" ]; then echo "$T" >&2; else trap 'rm -rf -- "$T"' EXIT; fi
rm -f "$out"
minor=${gccver#*2.}; minor=${minor%%.*}
defs="-D__GNUC__=2 -D__GNUC_MINOR__=$minor -Dmips -DMIPSEL -D_mips -D__mips__ -D__MIPSEL__ -D_MIPSEL -D__mips -D__MIPSEL -D_LANGUAGE_C -DLANGUAGE_C -D__LANGUAGE_C -D_R3000 -D__R3000 -D__R3000__"
if [ -z "$GNU_CPP" ]; then
    GNU_CPP=cpp
    for v in $(seq 20 -1 10); do command -v cpp-$v > /dev/null && { GNU_CPP=cpp-$v; break; }; done
fi
case "$in" in
*.c)
    $GNU_CPP -ffreestanding -undef $defs -D__OPTIMIZE__ -fmacro-prefix-map="$(dirname "$in")/=" -I./include -I"$(dirname "$in")" $cppflags "$in" "$T/a.i"
    if [ -n "$FUNCS" ]; then "$PY" -I tools/cfunc.py "$T/a.i" "$FUNCS" > "$T/f.i" && mv "$T/f.i" "$T/a.i"; fi
    ${CC1:-$G/cc1} -quiet $cc1flags "$T/a.i" -o "$T/a.s"
    tr -d '\r' < "$T/a.s" > "$T/b.s" && mv "$T/b.s" "$T/a.s"
    ;;
*.s)
    adefs=$(echo "$defs" | sed "s/LANGUAGE_C/LANGUAGE_ASSEMBLY/g")
    $GNU_CPP -ffreestanding -x assembler-with-cpp -undef $adefs -I./include -I"$(dirname "$in")" $cppflags "$in" "$T/a.s"
    ;;
esac
$AS -G 0 -o "$T/a.o" "$T/a.s"
"$PY" -I tools/ecoff2psyq.py --fmt=41 "$T/a.o" "$out"
