#!/usr/bin/python3

# signatures:
# https://github.com/lab313ru/psx_psyq_signatures

import json

preamble ="""
options:
  platform:     psx
  basename:     main
  base_path:    .
  build_path:   build/us
  target_path:  libsnd_linked/build/main.exe
  asm_path:     asm/us/main
  asset_path:   assets/main
  src_path:     src/main
  ld_script_path: build/us/main.ld
  compiler:     GCC
  symbol_addrs_path:    config/symbols.libsnd.txt
  undefined_funcs_auto_path: config/undefined_funcs_auto.libsnd.main.txt
  undefined_syms_auto_path:  config/undefined_syms_auto.libsnd.main.txt
  find_file_boundaries: yes
  use_legacy_include_asm: no
  migrate_rodata_to_functions: no
  asm_jtbl_label_macro: jlabel
  section_order:
    - ".rodata"
    - ".text"
    - ".data"
segments:
  - name: main
    type: code
    start: 0x00000800
    vram:  0x80010000
    subalign: 4
    subsegments:
"""

def match_signature(data, sig: str):
    if len(sig) == 0:
        return 0

    tokens = []
    for ch in sig.split(" "):
        if ch == " " or len(ch) == 0:
            continue
        if ch == "??":
            tokens.append(None)
        else:
            tokens.append(int(ch, 16))

    n_tokens = len(tokens)
    i = 0
    while i + n_tokens < len(data):
        start = i
        found = True
        for t in tokens:
            ch = data[i]
            i += 1
            if t == None:
                continue
            if t != ch:
                found = False
                break
        if found:
            return start
    return -1


def parse_libsig(lib_name, path, data):
    res = []
    with open(path, "r") as f:
        lib = json.load(f)

    for obj in lib:
        if "sig" not in obj:
            continue
        sig = obj["sig"]
        offset = match_signature(data, sig)
        if offset < 0:
            continue
        obj_name = str(obj["name"]).lower().replace(".obj", "")
        r = dict()
        r["lib"] = lib_name
        r["obj"] = obj_name
        r["data"] = obj
        r["off"] = offset
        res.append(r)
    return res

with open("./libsnd_linked/build/main.exe", "rb") as f:
    data = f.read()

res = []
for lib_name in [
    # "libapi",
    # "libc2",
    # "libcard",
    # "libcd",
    # "libc",
    # "libcomb",
    # "libetc",
    # "libgpu",
    # "libgs",
    # "libgte",
    # "libgun",
    # "libmath",
    # "libpress",
    "libsnd",
    # "libsn",
    # "libspu",
    # "libtap",
]:
    sig_path = f"./psx_psyq_signatures/350/{lib_name.upper()}.LIB.json"
    res.extend(parse_libsig(lib_name, sig_path, data))
for obj_name in ["2mbyte", "8mbyte", "none2", "none3"]:
    sig_path = f"./psx_psyq_signatures/350/{obj_name.upper()}.OBJ.json"
    res.extend(parse_libsig(obj_name, sig_path, data))

for r in sorted(res, key=lambda item: item["off"]):
    print(r["obj"], hex(r["off"]))

with open('splat.yaml', 'w') as file:
    file.write(preamble)  

    # print splat subsegment
    for r in sorted(res, key=lambda item: item["off"]):
        lib_name = r["lib"]
        obj_name = r["obj"]
        obj = r["data"]
        offset = r["off"]
        # print(f"- [0x{offset:X}, c, psxsdk/{lib_name}, {obj_name}]")
        file.write(f"      - [0x{offset:X}, c, psxsdk/{lib_name}/{obj_name}]\n")
    file.write(f"  - [{hex(len(data))}]")

address_map = {}

with open('./libsnd_linked/build/main.map', 'r') as file:
    start_parsing = False
    for line in file:
        if start_parsing:
            # Split the line into address and name
            try:
                address, name = line.split(maxsplit=1)
            except:
                break
            address = int(address, 16)
            name = name.strip()
            address_map[address] = name
        elif "Address" in line and "Names in address order" in line:
            # Start parsing when the header is found
            start_parsing = True

with open('config/symbols.libsnd.txt', 'w') as file:
    for address, name in address_map.items():
        file.write(f"{name} = {hex(address)};\n")
