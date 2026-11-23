#!/usr/bin/env python3
"""Cook the Bonnie Studios mark for the layer's boot splash (psyq/src/splash.rs) from the SDK.

    bonnie_mark.py LOGO.rs OUT.bin

LOGO.rs is the pinned SDK's sdk/crates/psx-splash/src/logo.rs (build/psoxide, the clone
psyq/build.sh makes): the mark the SDK's own splash draws, 128 by 128 at 4 bits a pixel, LZ
coded, and its 16-colour palette. The mark is decoded here as that file's `expand` documents
and checked against the file's own FNV-1a, so the layer draws exactly the SDK's mark and
never keeps a copy of it.

OUT.bin, little-endian 16-bit words: the 16 palette colours, then the 4,096 words of the
picture (32 words a row, 128 rows, four pixels a word, the leftmost in the low nibble).
"""
import re
import struct
import sys

WORDS = 4096


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    src = open(sys.argv[1]).read()
    pal = re.search(r'static PALETTE: \[u16; 16\] = \[(.*?)\];', src, re.S).group(1)
    pal = [int(v, 16) for v in re.findall(r'0x[0-9a-fA-F]+', pal)]
    coded = re.search(r'static CODED: \[u8; (\d+)\] = \[(.*?)\];', src, re.S)
    data = bytes(int(v, 16) for v in re.findall(r'0x[0-9a-fA-F]+', coded.group(2)))
    fnv = int(re.search(r'const FNV1A: u64 = (0x[0-9a-f_]+);', src).group(1).replace('_', ''), 16)
    assert len(pal) == 16 and len(data) == int(coded.group(1)), 'logo.rs: unexpected layout'

    out, nxt, flags = [], 0, 0
    while len(out) < WORDS:
        if flags < 2:
            flags = 0x100 | data[nxt]
            nxt += 1
        if flags & 1:
            out.append(data[nxt] | data[nxt + 1] << 8)
            nxt += 2
        else:
            lo, hi = data[nxt], data[nxt + 1]
            nxt += 2
            length, back = (hi >> 1) + 2, ((hi & 1) << 8) | lo
            for _ in range(length):
                out.append(out[len(out) - back])
        flags >>= 1
    out = out[:WORDS]

    h = 0xcbf29ce484222325
    for word in out:
        for byte in struct.pack('<H', word):
            h = ((h ^ byte) * 0x100000001b3) & 0xffffffffffffffff
    if h != fnv:
        sys.exit(f'{sys.argv[1]}: decoded mark hashes {h:#x}, the file says {fnv:#x}')
    open(sys.argv[2], 'wb').write(struct.pack('<16H', *pal) + struct.pack(f'<{WORDS}H', *out))


main()
