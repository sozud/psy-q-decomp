#!/usr/bin/env python3
import struct, sys

E = '>'

SEC_ID = {'.text': 0xF000, '.data': 0xF001, '.bss': 0xF002, '.sdata': 0xF003,
          '.rdata': 0xF004, '.sbss': 0xF005}
SEC_HDRS = [(0xF004, '.rdata'), (0xF000, '.text'), (0xF001, '.data'),
            (0xF003, '.sdata'), (0xF005, '.bss'), (0xF002, '.sbss')]
RSN = {1: '.text', 2: '.rdata', 3: '.data', 4: '.sdata', 5: '.sbss', 6: '.bss'}
SC = {1: '.text', 2: '.data', 3: '.bss', 13: '.sdata', 14: '.sbss', 15: '.rdata'}
SC_NIL, SC_UNDEF, SC_COMMON = 0, 6, 17
R_REFWORD, R_JMPADDR, R_REFHI, R_REFLO = 2, 3, 4, 5
PATCH_TYPE = {R_REFWORD: 16, R_JMPADDR: 74, R_REFHI: 82, R_REFLO: 84}


def parse_ecoff(b):
    E = '<' if struct.unpack_from('<H', b, 0)[0] in (0x160, 0x162) else '>'
    magic, nscns, _t, symptr, _n, opthdr, _f = struct.unpack_from(E + 'HHiiiHH', b, 0)
    if magic not in (0x160, 0x162):
        raise SystemExit('not a MIPS ECOFF object (magic %#x)' % magic)
    secs = []
    p = 20 + opthdr
    for _ in range(nscns):
        name = b[p:p + 8].split(b'\0')[0].decode()
        _pa, vaddr, size, scnptr, relptr, _l, nreloc, _nl, _fl = \
            struct.unpack_from(E + 'iiiiiiHHi', b, p + 8)
        relocs = []
        for j in range(nreloc):
            va, w = struct.unpack_from(E + 'iI', b, relptr + 8 * j)
            if E == '<':
                relocs.append((va, w & 0xFFFFFF, (w >> 27) & 0xF, w >> 31))
            else:
                relocs.append((va, w >> 8, (w >> 1) & 0xF, w & 1))
        secs.append(dict(name=name, vaddr=vaddr, size=size,
                         data=b[scnptr:scnptr + size] if scnptr else None,
                         relocs=relocs))
        p += 40
    ext = []
    if symptr:
        h = struct.unpack_from(E + 'hh' + 'i' * 23, b, symptr)
        iss_max, cb_ss_ext = h[17], h[18]
        iext_max, cb_ext = h[23], h[24]
        se = b[cb_ss_ext:cb_ss_ext + iss_max]
        for i in range(iext_max):
            o = cb_ext + 16 * i
            iss, value, w = struct.unpack_from(E + 'iiI', b, o + 4)
            if E == '<':
                st, sc = w & 0x3F, (w >> 6) & 0x1F
            else:
                st, sc = (w >> 26) & 0x3F, (w >> 21) & 0x1F
            ext.append(dict(name=se[iss:se.index(b'\0', iss)].decode('latin1'),
                            value=value, st=st, sc=sc))
    return secs, ext


def pstr(s):
    s = s.encode('latin1')
    return bytes([len(s)]) + s


def expr(addend, kind, idx):
    base = bytes([2 if kind == 'sym' else 4]) + struct.pack('<H', idx)
    if addend == 0:
        return base
    addend &= 0xFFFFFFFF
    if FMT == '41' and addend & 0x80000000:
        return b'\x2e\x00' + struct.pack('<I', -addend & 0xFFFFFFFF) + base
    return b'\x2c\x00' + struct.pack('<I', addend) + base


FMT = None


def imm16(data, off):
    return struct.unpack_from('<I', data, off)[0] & 0xFFFF


SEC_ID_41 = {'.rdata': 0xF001, '.text': 0xF002, '.data': 0xF003, '.sdata': 0xF004,
             '.sbss': 0xF005, '.bss': 0xF006}
SEC_HDRS_41 = [(v, k) for k, v in SEC_ID_41.items()]


def convert(b, fmt=None):
    global SEC_ID, FMT
    FMT = fmt
    secs, ext = parse_ecoff(b)
    byname = {s['name']: s for s in secs}
    hdrs = SEC_HDRS
    if fmt == '41':
        SEC_ID, hdrs = SEC_ID_41, SEC_HDRS_41
    out = bytearray(b'LNK\x02')
    out += b'\x2e\x07'
    for sid, name in hdrs:
        out += b'\x10' + struct.pack('<HHB', sid, 0, 8) + pstr(name)
    for i, e in enumerate(ext):
        n = i + 1
        sc = e['sc']
        if sc == SC_NIL:
            continue
        if sc == SC_UNDEF:
            out += b'\x0e' + struct.pack('<H', n) + pstr(e['name'])
        elif sc == SC_COMMON:
            out += b'\x30' + struct.pack('<HHI', n, SEC_ID['.bss'], e['value']) + pstr(e['name'])
        elif sc in SC:
            v = e['value'] - (byname[SC[sc]]['vaddr'] if fmt == '41' else 0)
            out += b'\x0c' + struct.pack('<HHI', n, SEC_ID[SC[sc]], v) + pstr(e['name'])
        else:
            raise SystemExit('unsupported storage class %d for %s' % (sc, e['name']))
    for s in secs:
        if s['size'] == 0 and (fmt != '41' or s['name'] in ('.bss', '.sbss')):
            continue
        sid = SEC_ID.get(s['name'])
        if sid is None:
            raise SystemExit('unsupported section ' + s['name'])
        out += b'\x06' + struct.pack('<H', sid)
        if s['size'] == 0:
            continue
        if s['data'] is None:
            out += b'\x08' + struct.pack('<I', s['size'])
            continue
        data = s['data']
        rel = s['relocs']
        patches = []
        for k, (va, symndx, typ, extern) in enumerate(rel):
            off = va - s['vaddr']
            w = struct.unpack_from('<I', data, off)[0]
            if typ == R_REFWORD:
                val = w
            elif typ == R_JMPADDR:
                val = (w & 0x3FFFFFF) << 2
            elif typ == R_REFHI:
                if fmt == '41':
                    nxt = [r for r in rel[k + 1:] if r[2] == R_REFLO and r[1] == symndx and r[3] == extern][:1]
                else:
                    nxt = rel[k + 1:k + 2]
                lo = imm16(data, nxt[0][0] - s['vaddr']) if nxt else 0
                val = ((w & 0xFFFF) << 16) + (lo - 0x10000 if lo & 0x8000 else lo)
            elif typ == R_REFLO:
                if fmt == '41':
                    prv = [r for r in rel[:k] if r[2] == R_REFHI and r[1] == symndx and r[3] == extern][-1:]
                else:
                    prv = rel[k - 1:k] if k > 0 else []
                hi = imm16(data, prv[0][0] - s['vaddr']) if prv else 0
                lo = w & 0xFFFF
                val = (hi << 16) + (lo - 0x10000 if lo & 0x8000 else lo)
            else:
                raise SystemExit('unsupported relocation type %d at %s+%#x' % (typ, s['name'], off))
            if extern and ext[symndx]['sc'] in SC:
                e_ = ext[symndx]
                tname = SC[e_['sc']]
                e = expr(val + e_['value'] - byname[tname]['vaddr'], 'sec', SEC_ID[tname])
            elif extern:
                e = expr(val, 'sym', symndx + 1)
            else:
                tname = RSN[symndx]
                e = expr(val - byname[tname]['vaddr'], 'sec', SEC_ID[tname])
            patches.append((off, bytes([PATCH_TYPE[typ]]), e))
        for o in range(0, len(data), 32768):
            chunk = data[o:o + 32768]
            out += b'\x02' + struct.pack('<H', len(chunk)) + chunk
            for off, t, e in patches:
                if o <= off < o + 32768:
                    out += b'\x0a' + t + struct.pack('<H', off - o) + e
    out += b'\x00'
    return bytes(out)


def main():
    args = sys.argv[1:]
    fmt = None
    if args and args[0].startswith('--fmt='):
        fmt = args.pop(0)[6:]
    if len(args) != 2:
        raise SystemExit(__doc__.split('\n\n')[1])
    with open(args[0], 'rb') as f:
        b = f.read()
    with open(args[1], 'wb') as f:
        f.write(convert(b, fmt))


if __name__ == '__main__':
    main()
