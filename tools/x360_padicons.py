#!/usr/bin/env python3
"""Make 360X's pad button icons header from their drawing.

    x360_padicons.py ART.txt OUT.h [PREVIEW.png]

ART.txt is 360x/art/padicons.txt: a palette (character, colour index, r g b) and
icons drawn one character per pixel, 16 rows each (the font cell's height). The
icons are packed left to right, each at an even x, into one strip of the font
page, starting at x 0 of row PADICON_PAGE_Y (192, the first of the four rows of
cells FONTS.TIM leaves empty). OUT.h holds the strip as 4-bit pixels (low nibble
first, as the GPU reads them, once GENERAL.C has unpacked them), the palette as
GPU colours, and each icon's x, width and fourth colour. The strip is kept at 2
bits a pixel to save the 2 MB program half a kilobyte: every icon uses at most
four colours, transparent (0), outline (k), body (b) and one more (its symbol
or its letters), so a pixel is 0 to 3 and 3 means the icon's own fourth colour.
GENERAL.C (Pad_Icons_Into_Fonts) unpacks it into the font page before the page
is loaded (Resident_Tims). PREVIEW.png, when given, is the strip at 4x on a grey
ground.
"""
import sys

PAGE_Y = 192          # FONTS.TIM rows 192 to 255 hold no glyph
ROWS = 16


def gpu_colour(r, g, b):
    c = (r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10)
    return c


def parse(path):
    pal = {}
    rgb = {}
    icons = []
    mode = None
    cur = None
    for raw in open(path):
        line = raw.rstrip('\n')
        if line.startswith('#') or not line.strip():
            if cur is not None and cur[1]:
                icons.append(cur)
                cur = None
            continue
        if line == 'palette':
            mode = 'pal'
            continue
        if line.startswith('icon '):
            if cur is not None:
                icons.append(cur)
            cur = (line.split()[1], [])
            mode = 'icon'
            continue
        if mode == 'pal':
            ch, idx, r, g, b = line.split()
            pal[ch] = int(idx)
            rgb[int(idx)] = (int(r), int(g), int(b))
        elif mode == 'icon':
            cur[1].append(line)
    if cur is not None and cur[1]:
        icons.append(cur)
    for name, rows in icons:
        if len(rows) != ROWS:
            sys.exit(f'{name}: {len(rows)} rows, want {ROWS}')
        if len({len(r) for r in rows}) != 1:
            sys.exit(f'{name}: rows of different widths')
        for r in rows:
            for ch in r:
                if ch not in pal:
                    sys.exit(f'{name}: character {ch!r} is not in the palette')
    if rgb.get(0) != (0, 0, 0):
        sys.exit('index 0 must be the transparent colour 0 0 0')
    for i, c in rgb.items():
        if i and gpu_colour(*c) == 0:
            sys.exit(f'index {i} would be transparent (black)')
    return pal, rgb, icons


def main():
    art, out = sys.argv[1], sys.argv[2]
    pal, rgb, icons = parse(art)
    placed = []
    x = 0
    for name, rows in icons:
        w = len(rows[0])
        placed.append((name, x, w, rows))
        x += w + (w & 1)
    width = x + (x & 1)
    if width > 256:
        sys.exit(f'icons are {width} pixels wide; the page is 256')
    strip = [[0] * width for _ in range(ROWS)]
    for name, x0, w, rows in placed:
        for y, r in enumerate(rows):
            for i, ch in enumerate(r):
                strip[y][x0 + i] = pal[ch]
    first = next(y for y in range(ROWS) if any(strip[y]))
    last = max(y for y in range(ROWS) if any(strip[y]))
    base = {0: 0, pal['k']: 1, pal['b']: 2}
    fourth = []
    codes = [[0] * width for _ in range(ROWS)]
    for name, x0, w, rows in placed:
        extra = sorted({strip[y][x] for y in range(ROWS) for x in range(x0, x0 + w)} - set(base))
        if len(extra) > 1:
            sys.exit(f'{name}: more than one colour besides transparent, outline and body: {extra}')
        fourth.append(extra[0] if extra else pal['w'])
        for y in range(ROWS):
            for x in range(x0, x0 + w):
                v = strip[y][x]
                codes[y][x] = base.get(v, 3)
    cols = (width + 3) // 4
    with open(out, 'w') as f:
        f.write('/* Made by tools/x360_padicons.py from 360x/art/padicons.txt. Do not edit. */\n')
        f.write(f'#define PADICON_PAGE_Y\t\t{PAGE_Y}\n')
        f.write(f'#define PADICON_FIRST_ROW\t{first}\t/* ROWS OF THE CELL THE STRIP HOLDS */\n')
        f.write(f'#define PADICON_STRIP_ROWS\t{last - first + 1}\n')
        f.write(f'#define PADICON_STRIP_PIXELS\t{width}\n')
        f.write(f'#define PADICON_STRIP_BYTES\t{cols}\t/* 2 BITS A PIXEL, THE FIRST PIXEL IN THE LOW BITS */\n')
        f.write(f'#define PADICON_COUNT\t\t{len(placed)}\n')
        f.write(f'#define PADICON_OUTLINE\t\t{pal["k"]}\t/* PALETTE INDEX OF CODE 1 */\n')
        f.write(f'#define PADICON_BODY\t\t{pal["b"]}\t/* PALETTE INDEX OF CODE 2 */\n')
        for n, (name, x0, w, _) in enumerate(placed):
            f.write(f'#define PADICON_{name}\t{n}\n')
        f.write('static const unsigned short padicon_pal[16]={')
        f.write(','.join(f'0x{gpu_colour(*rgb.get(i, (0, 0, 0))):04x}' for i in range(16)))
        f.write('};\n')
        f.write('static const unsigned char padicon_xwc[PADICON_COUNT][3]={\t/* x, WIDTH, PALETTE INDEX OF CODE 3 */\n')
        f.write(','.join(f'{{{x0},{w},{c}}}' for (_, x0, w, _), c in zip(placed, fourth)))
        f.write('};\n')
        f.write('static const unsigned char padicon_strip[PADICON_STRIP_ROWS][PADICON_STRIP_BYTES]={\n')
        for y in range(first, last + 1):
            row = codes[y] + [0] * (cols * 4 - width)
            f.write('{' + ','.join(str(row[i] | (row[i + 1] << 2) | (row[i + 2] << 4) | (row[i + 3] << 6)) for i in range(0, cols * 4, 4)) + '},\n')
        f.write('};\n')
    if len(sys.argv) > 3:
        from PIL import Image
        im = Image.new('RGB', (width, ROWS), (90, 110, 90))
        for y in range(ROWS):
            for xx in range(width):
                v = strip[y][xx]
                if v:
                    im.putpixel((xx, y), rgb[v])
        im.resize((width * 4, ROWS * 4), Image.NEAREST).save(sys.argv[3])


if __name__ == '__main__':
    main()
