#!/usr/bin/env python3
"""Pack the PSX Archive logo for the boot splash's credit (psyq/src/splash.rs).

    psxarchive_logo.py LOGO.png OUT.bin

LOGO.png is psyq/art/psxarchive-logo.png: an indexed (palette) PNG (1 to 8 bits
a pixel), at the size it is drawn on the 320 by 240 screen (at most 254 by 200, an
even width). It is drawn on black, so palette entry 0 should be the background.
The art is that file as committed; this script only packs it, nothing is
quantised or scaled here, so the output depends on the file alone.

OUT.bin, little-endian, every part a whole number of 16-bit words:
    width, height             u16 each
    palette                   256 GPU colours (u16, 5 bits each of r, g, b);
                              entries the PNG lacks are 0
    pixels                    width * height bytes, one palette index each,
                              rows top to bottom (the GPU's 8-bit texture
                              layout: two pixels a VRAM word, the left one low)

Standard library only (zlib), so the build needs no imaging package.
"""
import struct
import sys
import zlib


def read_png(path):
    data = open(path, 'rb').read()
    if data[:8] != b'\x89PNG\r\n\x1a\n':
        sys.exit(f'{path}: not a PNG')
    pos, idat, plte, trns = 8, b'', None, b''
    while pos < len(data):
        length, kind = struct.unpack('>I4s', data[pos:pos + 8])
        body = data[pos + 8:pos + 8 + length]
        pos += 12 + length
        if kind == b'IHDR':
            w, h, depth, colour, _, _, interlace = struct.unpack('>IIBBBBB', body)
        elif kind == b'PLTE':
            plte = body
        elif kind == b'tRNS':
            trns = body
        elif kind == b'IDAT':
            idat += body
    if colour != 3 or depth not in (1, 2, 4, 8) or interlace:
        sys.exit(f'{path}: must be an indexed PNG, not interlaced')
    raw = zlib.decompress(idat)
    row_bytes = (w * depth + 7) // 8
    rows, prev, stride = [], bytearray(row_bytes), row_bytes + 1
    for y in range(h):
        line = raw[y * stride:(y + 1) * stride]
        f, cur = line[0], bytearray(line[1:])
        for x in range(row_bytes):
            a = cur[x - 1] if x else 0
            b = prev[x]
            c = prev[x - 1] if x else 0
            if f == 1:
                cur[x] = (cur[x] + a) & 255
            elif f == 2:
                cur[x] = (cur[x] + b) & 255
            elif f == 3:
                cur[x] = (cur[x] + (a + b) // 2) & 255
            elif f == 4:
                p = a + b - c
                pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
                cur[x] = (cur[x] + (a if pa <= pb and pa <= pc else b if pb <= pc else c)) & 255
        per = 8 // depth
        rows.append(bytes((cur[x // per] >> (8 - depth * (x % per + 1))) & ((1 << depth) - 1) for x in range(w)))
        prev = cur
    pal = [tuple(plte[i:i + 3]) for i in range(0, len(plte), 3)]
    return w, h, pal, trns, b''.join(rows)


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    w, h, pal, trns, pixels = read_png(sys.argv[1])
    if w % 2 or w > 254 or h > 200:
        sys.exit(f'{sys.argv[1]}: {w} by {h}; the width must be even and at most 254, the height at most 200')
    words = []
    for i in range(256):
        if i >= len(pal) or (i < len(trns) and trns[i] == 0):
            words.append(0)
            continue
        r, g, b = pal[i]
        c = (r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10)
        # 0x0000 is transparent to the GPU: a colour that rounds to black stays opaque black.
        words.append(c or 0x8000)
    out = struct.pack('<HH', w, h) + struct.pack('<256H', *words) + pixels
    open(sys.argv[2], 'wb').write(out)


main()
