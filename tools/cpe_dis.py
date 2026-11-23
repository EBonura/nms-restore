#!/usr/bin/env python3
"""Disassemble functions of an SN CPE (dev kit executable) by name.

    cpe_dis.py POOL.CPE POOL.MAP get_pad [more names...]
    cpe_dis.py POOL.CPE POOL.MAP --image out.bin   (flat image from the lowest load address)
    cpe_dis.py MAIN.CPE MAIN.SYM VblCallback       (SN .SYM symbols, tools/sn_sym.py)

The CPE is read chunk by chunk (01h = load bytes at an address, 03h = set a
register, 08h = select unit, 00h = end). Function bounds come from the MAP's
address list: a function runs up to the next symbol. Disassembly is by
mipsel-none-elf-objdump. For reading what NMS's 1996 build did (observed
behaviour); nothing here is copied into the restoration.
"""
import os
import struct
import subprocess
import sys
import tempfile


def read_cpe(path):
    data = open(path, 'rb').read()
    if data[:4] != b'CPE\x01':
        sys.exit(f'{path}: not a CPE file')
    pos, chunks, regs = 4, [], {}
    while pos < len(data):
        kind = data[pos]
        pos += 1
        if kind == 0:
            break
        if kind == 1:
            addr, length = struct.unpack_from('<II', data, pos)
            pos += 8
            chunks.append((addr, data[pos:pos + length]))
            pos += length
        elif kind == 3:
            reg, value = struct.unpack_from('<HI', data, pos)
            regs[reg] = value
            pos += 6
        elif kind == 8:
            pos += 1
        else:
            sys.exit(f'{path}: unknown chunk {kind:#x} at {pos - 1:#x}')
    return chunks, regs


def flat_image(chunks):
    base = min(a for a, _ in chunks)
    end = max(a + len(d) for a, d in chunks)
    image = bytearray(end - base)
    for addr, blob in chunks:
        image[addr - base:addr - base + len(blob)] = blob
    return base, bytes(image)


def read_map(path):
    syms = {}
    if path.upper().endswith('.SYM'):
        sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
        from sn_sym import read_sym
        for addr, name in read_sym(path)[0]:
            syms.setdefault(name, addr)
        return syms
    for line in open(path, errors='replace'):
        parts = line.split()
        if len(parts) == 2 and len(parts[0]) == 8:
            try:
                syms.setdefault(parts[1], int(parts[0], 16))
            except ValueError:
                pass
    return syms


def main():
    if len(sys.argv) < 4:
        sys.exit(__doc__)
    cpe, mapf = sys.argv[1], sys.argv[2]
    chunks, regs = read_cpe(cpe)
    base, image = flat_image(chunks)
    if sys.argv[3] == '--image':
        open(sys.argv[4], 'wb').write(image)
        print(f'base {base:#x} size {len(image):#x} regs {regs}')
        return
    syms = read_map(mapf)
    addrs = sorted(set(syms.values()))
    by_addr = {}
    for name, a in syms.items():
        by_addr.setdefault(a, name)
    with tempfile.TemporaryDirectory() as tmp:
        for name in sys.argv[3:]:
            start = syms[name]
            nxt = min((a for a in addrs if a > start), default=start + 0x400)
            nxt = min(nxt, start + 0x2000)
            blob = os.path.join(tmp, 'f.bin')
            open(blob, 'wb').write(image[start - base:nxt - base])
            out = subprocess.run(['mipsel-none-elf-objdump', '-D', '-b', 'binary', '-m', 'mips:3000', '-EL',
                                  f'--adjust-vma={start:#x}', blob], capture_output=True, text=True).stdout
            print(f'=== {name} {start:#x}..{nxt:#x}')
            for line in out.splitlines():
                if ':\t' in line:
                    # name call targets
                    for word in line.split():
                        if word.startswith('0x8') and len(word) == 10:
                            a = int(word, 16)
                            if a in by_addr:
                                line += f'   <{by_addr[a]}>'
                    print(line)


if __name__ == '__main__':
    main()
