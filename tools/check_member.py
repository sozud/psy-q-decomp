#!/usr/bin/env python3
import os
import struct
import sys


def members(path):
    data = open(path, 'rb').read()
    assert data[:4] == b'LIB\x01', path
    out, p = {}, 4
    while p + 20 <= len(data):
        name = data[p:p + 8].decode().rstrip()
        hdr, size = struct.unpack_from('<II', data, p + 12)
        out[name] = data[p + hdr:p + size]
        p += size
    return out


lib, obj = sys.argv[1:3]
name = os.path.splitext(os.path.basename(obj))[0].upper()
want = members(lib).get(name)
if want is None:
    sys.exit(f'{name} not in {lib}')
got = open(obj, 'rb').read()
if got != want:
    n = next((i for i, (a, b) in enumerate(zip(got, want)) if a != b), min(len(got), len(want)))
    sys.exit(f'{obj}: differs from {lib} {name} at byte {n:#x} (sizes {len(got):#x} / {len(want):#x})')
