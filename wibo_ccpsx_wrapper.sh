#!/bin/bash

set -e

if [ "$#" -lt 2 ]; then
    echo "Usage: $0 input output [compiler flags...]" >&2
    exit 1
fi

repo_dir=$(pwd -P)
input_path=$(realpath "$1")
output_path="$repo_dir/$2"
shift 2

compiler_args=()
for arg in "$@"; do
    case "$arg" in
        -I./*) compiler_args+=("-I$repo_dir/${arg#-I./}") ;;
        *) compiler_args+=("$arg") ;;
    esac
done

temp_dir=$(mktemp -d)
trap 'rm -rf -- "$temp_dir"' EXIT

cd "$temp_dir"
WIBO_DEBUG=0 \
PSYQ_PATH="$repo_dir/build/4.0" \
COMPILER_PATH="$repo_dir/build/4.0" \
C_INCLUDE_PATH="$repo_dir/include" \
"$repo_dir/build/4.0/wibo" "$repo_dir/build/4.0/CCPSX.EXE" \
    "$input_path" "${compiler_args[@]}" "-o$output_path"
