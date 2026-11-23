#!/usr/bin/env python3
"""Check NMS's assembler, as rebuilt by tools/asmpsx2gas (Rust), against the
1996 build: every global function in the object is compared word for word
with the same function in POOL.CPE (addresses from POOL.MAP). Fields the
linker fills in (relocations: lui/%lo pairs, j/jal targets) are masked.

    asm_check.py OBJ.o POOL.CPE POOL.MAP

Prints one line per function: `same`, or the first differing words.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cpe_dis  # noqa: E402


def object_text(obj):
    """(bytes of .text, {global function: offset}, {offset: reloc type})."""
    tmp = obj + '.text.bin'
    subprocess.run(['mipsel-none-elf-objcopy', '-O', 'binary', '-j', '.text', obj, tmp], check=True)
    data = open(tmp, 'rb').read()
    os.remove(tmp)
    syms = {}
    for line in subprocess.run(['mipsel-none-elf-nm', obj], capture_output=True, text=True).stdout.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[1] == 'T':
            syms[parts[2]] = int(parts[0], 16)
    relocs = {}
    out = subprocess.run(['mipsel-none-elf-objdump', '-r', '-j', '.text', obj], capture_output=True, text=True).stdout
    for line in out.splitlines():
        m = re.match(r'^([0-9a-f]{8})\s+(R_MIPS_\w+)', line)
        if m:
            relocs[int(m.group(1), 16)] = m.group(2)
    return data, syms, relocs


def mask(word, reloc):
    if reloc == 'R_MIPS_26':
        return word & 0xFC000000
    if reloc in ('R_MIPS_HI16', 'R_MIPS_LO16'):
        return word & 0xFFFF0000
    return word


def main():
    obj, cpe, mapf = sys.argv[1:4]
    data, syms, relocs = object_text(obj)
    base, image = cpe_dis.flat_image(cpe_dis.read_cpe(cpe)[0])
    map_syms = cpe_dis.read_map(mapf)
    starts = sorted(syms.values()) + [len(data)]
    bad = 0
    for name, off in sorted(syms.items(), key=lambda kv: kv[1]):
        end = min(s for s in starts if s > off)
        if name not in map_syms:
            print(f'{name}: not in the 1996 map')
            continue
        addr = map_syms[name] - base
        diffs = []
        for i in range(off, end, 4):
            mine = int.from_bytes(data[i:i + 4], 'little')
            theirs = int.from_bytes(image[addr + i - off:addr + i - off + 4], 'little')
            r = relocs.get(i)
            if mask(mine, r) != mask(theirs, r):
                diffs.append(f'+{i - off:#x}: ours {mine:08x} 1996 {theirs:08x}')
        if diffs:
            bad += 1
            print(f'{name}: {len(diffs)} of {(end - off) // 4} words differ; ' + '; '.join(diffs[:4]))
        else:
            print(f'{name}: same ({(end - off) // 4} words)')
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
