#!/usr/bin/env python3
"""360X's sprite tables: NMS's TXT2SPR step, in Python.

    x360_txt2spr.py TXT OUT.SPR           compile one table
    x360_txt2spr.py --check DATA_ROOT      compile every GRAPHICS/LEVEL1 table
                                           that has a game copy in LEVEL1/ and
                                           compare it byte for byte
    x360_txt2spr.py --stage DATA_ROOT LOADDATA.C OUTDIR
                                           build the files LOADDATA.C loads
                                           from LEVEL2..9 (and the LEVEL1 files
                                           the 1996 LEVEL1/ folder lacks) into
                                           OUTDIR/LEVELn/

NMS's art pipeline, from the 1996 tree:
  GRAPHICS/LEVELn/X.FLC   the animation
  GRAPHICS/LEVELn/X.PSX   the converted PS1 image: a 16-colour palette, then
                          one run-length frame after another
  GRAPHICS/LEVELn/X.TXT   the frame table the converter wrote with it, as C:
                          one "SPRITE spr_Xk = { SPTRCAST image, SPTRCAST pal,
                          x,y, w,h, u,v, clut, tpage, flags};" line per frame
                          (image and pal are offsets into X.PSX)
  X.SPR                   the frame table compiled: GRAPHICS/TXT2SPR.BAT
                          compiles TXT2SPR.C, which includes PSC2PSB.H (the
                          SPRITE struct and flag names) and the .TXT between
                          the strings "STARTOFFILE" and "ENDOFFILE", and cuts
                          the bytes between them out of the executable
                          (chopfile /SSTARTOFFILE /EENDOFFILE)
  LEVELn/X.PSX, X.SPR     copied next to the game, which loads them
                          (LOADDATA.C: LoadFile, Init_Spr_List)

The SPRITE struct (PSC2PSB.H) is 24 bytes, little endian: ULONG image,
ULONG pal, UBYTE x, y, w, h, SHORT u, v, USHORT clut, tpage, flags, and two
bytes of padding. Every Mission 1 .SPR is its table of 24-byte entries
followed by one byte 0x0A (chopfile's cut); the padding is zero. --check
compiles the .TXT files and compares them with NMS's .SPR files: 28 of
LEVEL1/'s 30 tables come out byte for byte. The other two were not made by
this step alone: S_SURVS has no .TXT, and LEVEL1/S_VIL1 holds six frames,
the one-frame images S_VIL1..S_VIL6 of GRAPHICS/LEVEL1 put together with
hotspots (u,v) set by hand (-48,-59 where S_VIL2.TXT has -32,-46).
"""
import os
import re
import shutil
import struct
import sys

# PSC2PSB.H's flag names
FLAGS = {'COL16': 0, 'FLIP': 1, 'COL256': 2, 'COL16BIT': 4, 'BITSTREAM': 8, 'RUNLENGTH': 16,
         'FIRSTSPR': 32, 'RESPAGE': 64, 'LASTSPR': 128, 'NOIMAGE': 256, 'TEMPVRAM': 512,
         'DONTXORVRAM': 1024, 'MAPSPR': 2048, 'INVRAM': 4096, 'SPLIT': 8192}
LINE = re.compile(r'SPRITE\s+\w+\s*=\s*\{\s*SPTRCAST\s+(-?\d+)\s*,\s*SPTRCAST\s+(-?\d+)\s*,(.*)\}\s*;')
TAIL = b'\x0a'
KNOWN = {'S_VIL1.SPR': 'six vine images joined by hand (see above)'}


def flag_value(expr):
    total = 0
    for term in expr.replace(' ', '').split('+'):
        total += int(term) if term.lstrip('-').isdigit() else FLAGS[term]
    return total


def compile_txt(text):
    out = bytearray()
    n = 0
    for line in text.replace('\x1a', '').splitlines():
        if not line.strip():
            continue
        m = LINE.match(line.strip())
        if not m:
            raise ValueError(f'not a sprite line: {line!r}')
        f = [x.strip() for x in m.group(3).split(',')]
        if len(f) != 9:
            raise ValueError(f'expected 9 fields after the pointers: {line!r}')
        x, y, w, h, u, v, clut, tpage = (int(s) for s in f[:8])
        out += struct.pack('<IIBBBBhhHHHxx', int(m.group(1)), int(m.group(2)),
                           x, y, w, h, u, v, clut, tpage, flag_value(f[8]))
        n += 1
    if n == 0:
        raise ValueError('no sprite lines')
    last = struct.unpack_from('<H', out, len(out) - 4)[0]
    if not last & FLAGS['LASTSPR']:
        raise ValueError('the last entry has no LASTSPR flag')
    return bytes(out) + TAIL


def read_txt(path):
    return open(path, 'rb').read().decode('latin-1')


def check(root):
    game = os.path.join(root, 'LEVEL1')
    src = os.path.join(root, 'GRAPHICS', 'LEVEL1')
    same = differ = 0
    for n in sorted(os.listdir(game)):
        if not n.endswith('.SPR'):
            continue
        txt = os.path.join(src, n[:-4] + '.TXT')
        if not os.path.exists(txt):
            print(f'{n}: no .TXT in GRAPHICS/LEVEL1')
            continue
        built = compile_txt(read_txt(txt))
        nms = open(os.path.join(game, n), 'rb').read()
        psx_same = open(os.path.join(game, n[:-4] + '.PSX'), 'rb').read() == \
            open(os.path.join(src, n[:-4] + '.PSX'), 'rb').read()
        if n in KNOWN and built != nms:
            print(f'{n}: differs, as expected: {KNOWN[n]}')
        elif built == nms:
            same += 1
            print(f'{n}: identical ({len(nms)} bytes), .PSX {"identical" if psx_same else "DIFFERS"}')
        else:
            differ += 1
            print(f'{n}: DIFFERS (built {len(built)}, NMS {len(nms)} bytes)')
    print(f'{same} identical, {differ} different')
    return 1 if differ else 0


def stage(root, loaddata, outdir):
    """Every "../LEVELn/X.SPR" LOADDATA.C names and the 1996 LEVELn/ folder
    lacks is compiled from GRAPHICS/LEVELn/X.TXT, its X.PSX copied with it."""
    text = open(loaddata, 'rb').read().decode('latin-1')
    names = sorted(set(re.findall(r'"\.\./(LEVEL[1-9])/(\w+)\.(?:PSX|SPR)"', text)))  # LOADDATA.C names the .PSX, the .SPR beside it
    made = 0
    for folder, base in names:
        if os.path.exists(os.path.join(root, folder, base + '.SPR')):
            continue
        src = os.path.join(root, 'GRAPHICS', folder)
        dst = os.path.join(outdir, folder)
        os.makedirs(dst, exist_ok=True)
        spr = compile_txt(read_txt(os.path.join(src, base + '.TXT')))
        open(os.path.join(dst, base + '.SPR'), 'wb').write(spr)
        shutil.copyfile(os.path.join(src, base + '.PSX'), os.path.join(dst, base + '.PSX'))
        made += 1
    print(f'{outdir}: {made} sprite tables compiled')


def main():
    a = sys.argv[1:]
    if len(a) == 2 and a[0] == '--check':
        return check(a[1])
    if len(a) == 4 and a[0] == '--stage':
        return stage(a[1], a[2], a[3])
    if len(a) == 2:
        open(a[1], 'wb').write(compile_txt(read_txt(a[0])))
        return 0
    sys.exit(__doc__)


if __name__ == '__main__':
    sys.exit(main())
