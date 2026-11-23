#!/usr/bin/env python3
"""Read an SN Systems .SYM file (the debugger symbols next to a CPE).

    sn_sym.py MAIN.SYM            print 'address name' for every symbol, sorted
    sn_sym.py MAIN.SYM --funcs    also print function records (address, size, file)

The format is the publicly documented MND record stream: an 8-byte header,
then records of an address (u32) and a type byte. Types 1 and 2 are symbols
(length-prefixed name), 0x80..0x92 source-line and function/block records,
0x94/0x96 type definitions. Only what is needed to map names to addresses is
decoded; the rest is skipped by size.
"""
import struct
import sys


def read_sym(path):
    d = open(path, 'rb').read()
    if d[:3] != b'MND':
        sys.exit(f'{path}: not an SN symbol file')
    pos = 8
    syms, funcs = [], []

    def pstr(p):
        n = d[p]
        return d[p + 1:p + 1 + n].decode('latin-1'), p + 1 + n

    while pos + 5 <= len(d):
        addr, kind = struct.unpack_from('<IB', d, pos)
        pos += 5
        if kind in (1, 2):
            name, pos = pstr(pos)
            syms.append((addr, name))
        elif kind == 0x80 or kind == 0x8a:
            pass
        elif kind == 0x82:
            pos += 1
        elif kind == 0x84:
            pos += 2
        elif kind == 0x86:
            pos += 4
        elif kind == 0x88:
            pos += 4
            _, pos = pstr(pos)
        elif kind == 0x8c:
            fp, fsize, retreg, mask, maskoff, line = struct.unpack_from('<HIHIII', d, pos)
            pos += 20
            fname, pos = pstr(pos)
            func, pos = pstr(pos)
            funcs.append((addr, func, fsize, fname))
        elif kind in (0x8e, 0x90, 0x92):
            pos += 4
        elif kind == 0x94:
            pos += 8
            _, pos = pstr(pos)
        elif kind == 0x96:
            cls, typ, size, dims = struct.unpack_from('<HHIH', d, pos)
            pos += 10 + 4 * dims
            _, pos = pstr(pos)
            _, pos = pstr(pos)
        else:
            sys.exit(f'unknown record type {kind:#x} at {pos - 5:#x}')
    return syms, funcs


def main():
    syms, funcs = read_sym(sys.argv[1])
    for a, n in sorted(set(syms)):
        print(f'{a:08x} {n}')
    if '--funcs' in sys.argv:
        for a, n, s, f in sorted(funcs):
            print(f'func {a:08x} {s:6d} {n} {f}')


if __name__ == '__main__':
    main()
