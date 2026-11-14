#!/bin/bash
set -euo pipefail

VERSION=v1.0.0
URL=https://github.com/sozud/psy-q-compilers/releases/download/$VERSION

case "$(uname -s)" in
Linux) os=linux ;;
Darwin) os=macos ;;
*) echo "unsupported host $(uname -s)" >&2; exit 1 ;;
esac
case "$(uname -m)" in
x86_64|amd64) arch=x86_64 ;;
arm64|aarch64) arch=arm64 ;;
*) echo "unsupported arch $(uname -m)" >&2; exit 1 ;;
esac
case $os-$arch in
linux-x86_64) sha=f97512934167f5e8df9f1b0c5cb371747a54db73b4371a828a86a46857985b0d ;;
linux-arm64)  sha=6b52941501c7eeb447d868d4fe1ffd036e35961e6efa55baa94da710b561f99d ;;
macos-x86_64) sha=74e19008301aa12ceb383d5964cd98d549e11a85f3b41b437e0e8bcf42010519 ;;
macos-arm64)  sha=8de474aba0d78a6fc25f27483e101a89b16b6bf856e80fa8e3b230b62b8c14ba ;;
esac

ROOT=$(cd "$(dirname "$0")" && pwd)
OUT=$ROOT/build/gnu
STAMP=$OUT/.version
[ "$(cat "$STAMP" 2>/dev/null)" = "$VERSION-$os-$arch" ] && exit 0

name=psy-q-gnu-$os-$arch.tar.gz
tmp=$(mktemp -d)
trap 'rm -rf -- "$tmp"' EXIT
curl -sSfL -o "$tmp/$name" "$URL/$name"
if command -v sha256sum > /dev/null; then got=$(sha256sum "$tmp/$name"); else got=$(shasum -a 256 "$tmp/$name"); fi
[ "${got%% *}" = "$sha" ] || { echo "$name: sha256 mismatch" >&2; exit 1; }
mkdir -p "$OUT"
tar -xzf "$tmp/$name" -C "$OUT"
echo "$VERSION-$os-$arch" > "$STAMP"
echo "psy-q-compilers $VERSION ($os-$arch) -> build/gnu"
