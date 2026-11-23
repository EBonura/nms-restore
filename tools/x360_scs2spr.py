#!/usr/bin/env python3
"""360X's Saturn sprites (.SCS) as the PS1 sprite pipeline loads them.

    x360_scs2spr.py IN.SCS OUT.PSX OUT.SPR      convert one file
    x360_scs2spr.py --stage SATURN_ROOT OUTDIR   convert the files the PS1
                                                 build loads (STAGE below)
                                                 into OUTDIR/SATURN/
    x360_scs2spr.py --compare IN.SCS X.PSX X.SPR compare a Saturn file with
                                                 NMS's PS1 art of the same
                                                 creature, frame by frame

SATURN_ROOT is the Saturn tree (the unpacked 20SEPT96.ZIP): its SPRITES
folder holds the .SCS files. The tool reads data only; nothing of the
Saturn tree is copied into the repository.

The .SCS format, from NMS's Saturn source (VDP/VDP1_3.C DecodeSCS,
SHCODE/RUNLEN.S shRle2raw, SHCODE/BIT2RAW.S shBit2raw), all big endian:

  u16 frames, u16 clear colour
  frames x 7 longs: image offset, width, height, x offset, y offset,
                    slot size (the Saturn's VRAM slot class), compression
                    (0 run length, 1 bit stream)
  32 bytes: the 16-colour palette (15-bit colours, bit 15 set, the PS1's
            own bit layout)
  the frames' images: a long with the decoded length, then the data
  (run length: a signed count byte, positive n copies n+1 bytes, n<=0
  repeats the next byte 1-n times; bit stream: one mask bit per byte,
  most significant first, then the bytes whose bit is set, the rest 0).
  A decoded frame is height rows of width/2 bytes, two 4-bit pixels a byte,
  the left pixel in the high nibble. The Saturn draws pixel 0 as clear
  whatever the palette says (VDP1); the file's palette entry 0 is not a
  colour.

The PS1 pipeline (tools/x360_txt2spr.py, SPRITE.C) loads two files per
creature: X.PSX, a 16-colour palette (16 little-endian words, 0 clear) and
then one run-length frame after another (a little-endian long with the
encoded length, then the data, each frame on a 4-byte boundary; two 4-bit
pixels a byte, the LEFT pixel in the LOW nibble, as the PS1's 4-bit images
are), and X.SPR, the 24-byte SPRITE table (image offset, palette offset 0,
x y 0, w h, the hotspot u v, clut and tpage 0, flags) closed by the 0x0A
chopfile leaves. The Saturn's frame fields map across unchanged: width,
height, x offset to u, y offset to v (NMS's own PS1 art of the same
creatures has the same values: --compare), the images re-ordered nibble by
nibble and re-coded, the palette copied with entry 0 set to the PS1's clear
(0, the PS1 draws a colour of 0 as clear and any other value, 0x8000
included, as a colour).
"""
import os
import struct
import sys

# PSC2PSB.H's flag names (tools/x360_txt2spr.py)
FIRSTSPR, LASTSPR, RUNLENGTH, COL16 = 32, 128, 16, 0
TAIL = b'\x0a'

# the files the PS1 build loads, by output folder: tools wiring is in
# LOADDATA.C (360x/SOURCE). Saturn name -> PS1 name (8.3, at most 15 chars)
STAGE = (
    ['S_EXP1', 'S_EXP4', 'S_EXP6', 'S_EXP7', 'S_LASER', 'S_POD', 'S_SPFIRE'] +
    ['S_V%d_%d' % (m, k) for m, ks in ((2, (1, 2, 3, 4, 6)), (3, (1, 3, 4, 6)), (4, (1, 2, 3, 4, 6)),
                                       (5, (1, 2, 3, 4, 6)), (6, (1, 2, 3, 4, 5, 6))) for k in ks]
)
# every Saturn explosion; S_EXP2, S_EXP3 and S_EXP5 are converted by --stage-all
# (checked) but the game wires only what NMS's code spawns (docs/DECISIONS.md)
# frames kept, for files whose later frames no map range reaches: every
# lazerlN placement's ranges in LEVELn.BMF end at 20 (normal 0..20, left and
# right 20..20), and ai_lazer's own setanimrange stays inside them, while
# S_LASER has 41 frames; the other 20 would cost the PS1 about 14 KB of VRAM
# and 11 KB of RAM, and Mission 1's VRAM has no room for them
LIMIT = {'S_LASER': 21}
ALL_EXPLOSIONS = ['S_EXP%d' % n for n in range(1, 8)]


def rle_decode(d, off):
    n = struct.unpack('>I', d[off:off + 4])[0]
    p = off + 4
    out = bytearray()
    while n > 0:
        t = d[p]
        p += 1
        s = t - 256 if t >= 128 else t
        if s > 0:
            c = s + 1
            n -= c + 1
            out += d[p:p + c]
            p += c
        else:
            n -= 2
            out += bytes([d[p]]) * (1 - s)
            p += 1
    return bytes(out)


def bit_decode(d, off):
    n = struct.unpack('>I', d[off:off + 4])[0]
    m = (n - 1) // 8 + 1
    mask = d[off + 4:off + 4 + m]
    cols = d[off + 4 + m:]
    out = bytearray()
    ci = 0
    for i in range(n):
        if mask[i // 8] & (0x80 >> (i % 8)):
            out.append(cols[ci])
            ci += 1
        else:
            out.append(0)
    return bytes(out)


def read_scs(path):
    """-> (frames, palette words, clear colour); frames: dicts w, h, u, v,
    pix (h rows of w pixel values 0..15)."""
    d = open(path, 'rb').read()
    n, clear = struct.unpack('>HH', d[:4])
    frames = []
    for i in range(n):
        img, w, h, ox, oy, slot, cmp_ = struct.unpack('>7l', d[4 + i * 28:32 + i * 28])
        if w <= 0 or h <= 0:
            raise ValueError(f'{path}: frame {i} is empty ({w}x{h})')
        raw = (rle_decode, bit_decode)[cmp_](d, img)
        need = w // 2 * h
        raw = (raw + bytes(need))[:need]
        pix = []
        for y in range(h):
            row = raw[y * (w // 2):(y + 1) * (w // 2)]
            pix.append([v for b in row for v in (b >> 4, b & 15)])
        frames.append(dict(w=w, h=h, u=ox, v=oy, pix=pix))
    pal = list(struct.unpack('>16H', d[n * 28 + 4:n * 28 + 36]))
    return frames, pal, clear


def rle_encode(raw):
    """The PS1's Rle2raw code (SPRITE.C): temp > 0x80 repeats the next byte
    1-temp times (2..128), otherwise copies temp+1 bytes (1..128 here)."""
    out = bytearray()
    i = 0
    while i < len(raw):
        run = 1
        while i + run < len(raw) and raw[i + run] == raw[i] and run < 128:
            run += 1
        if run >= 2:
            out += bytes([(1 - run) & 255, raw[i]])
            i += run
            continue
        j = i
        while j < len(raw) and j - i < 128 and not (j + 1 < len(raw) and raw[j] == raw[j + 1]):
            j += 1
        if j == i:
            j = i + 1
        out += bytes([j - i - 1]) + raw[i:j]
        i = j
    return bytes(out)


def ps1_bytes(frame):
    """One frame's PS1 raw 4-bit image: the left pixel in the low nibble."""
    out = bytearray()
    for row in frame['pix']:
        for x in range(0, frame['w'], 2):
            out.append(row[x] | row[x + 1] << 4)
    return bytes(out)


def convert(frames, pal):
    """-> (PSX bytes, SPR bytes)."""
    for f in frames:
        if f['w'] % 4 or not 0 < f['w'] < 256 or not 0 < f['h'] < 256:
            raise ValueError('a frame size the PS1 pipeline cannot hold: %dx%d' % (f['w'], f['h']))
    words = [0] + [p if p else 0x8000 for p in pal[1:]]
    psx = bytearray(struct.pack('<16H', *words))
    where = {}
    spr = bytearray()
    for i, f in enumerate(frames):
        raw = ps1_bytes(f)
        key = (f['w'], f['h'], raw)
        if key not in where:
            enc = rle_encode(raw)
            while len(psx) % 4:
                psx.append(0)
            where[key] = len(psx)
            psx += struct.pack('<I', len(enc)) + enc
        flags = RUNLENGTH + COL16 + (FIRSTSPR if i == 0 else 0) + (LASTSPR if i == len(frames) - 1 else 0)
        spr += struct.pack('<IIBBBBhhHHHxx', where[key], 0, 0, 0, f['w'], f['h'], f['u'], f['v'], 0, 0, flags)
    while len(psx) % 4:
        psx.append(0)
    return bytes(psx), bytes(spr) + TAIL


def ps1_rle_decode(d, off):
    n = struct.unpack('<I', d[off:off + 4])[0]
    p = off + 4
    out = bytearray()
    while n > 0:
        t = d[p]
        p += 1
        if t > 0x80:
            n -= 2
            out += bytes([d[p]]) * (1 - t & 255)
            p += 1
        else:
            c = t + 1
            n -= c + 1
            out += d[p:p + c]
            p += c
    return bytes(out)


def ps1_bit_decode(d, off):
    n = struct.unpack('<I', d[off:off + 4])[0]
    m = (n - 1) // 8 + 1
    mask = d[off + 4:off + 4 + m]
    cols = d[off + 4 + m:]
    out = bytearray()
    ci = 0
    for i in range(n):
        if mask[i // 8] & (0x80 >> (i % 8)):
            out.append(cols[ci])
            ci += 1
        else:
            out.append(0)
    return bytes(out)


def read_ps1(psx_path, spr_path):
    """NMS's PS1 art -> (frames as read_scs gives them, palette words)."""
    psx = open(psx_path, 'rb').read()
    spr = open(spr_path, 'rb').read()
    pal = list(struct.unpack('<16H', psx[:32]))
    frames = []
    for i in range(len(spr) // 24):
        img, pl, x, y, w, h, u, v, clut, tp, fl = struct.unpack_from('<IIBBBBhhHHHxx', spr, i * 24)
        raw = (ps1_bit_decode if fl & 8 else ps1_rle_decode)(psx, img)
        pitch = w // 2
        need = pitch * h
        raw = (raw + bytes(need))[:need]
        pix = []
        for r in range(h):
            row = raw[r * pitch:(r + 1) * pitch]
            pix.append([q for b in row for q in (b & 15, b >> 4)])
        frames.append(dict(w=w, h=h, u=u, v=v, pix=pix))
    return frames, pal


def compare(scs, psx, spr):
    sf, sp, _ = read_scs(scs)
    pf, pp = read_ps1(psx, spr)
    same_pal = [0] + sp[1:] == [0] + pp[1:]
    print(f'{os.path.basename(scs)}: Saturn {len(sf)} frames, PS1 {len(pf)} frames; palettes '
          f'{"equal" if same_pal else "differ"} (entries 1..15)')
    n = min(len(sf), len(pf))
    ident = [i for i in range(n) if sf[i]['pix'] == pf[i]['pix'] and sf[i]['w'] == pf[i]['w']]
    hot = [i for i in range(n) if (sf[i]['u'], sf[i]['v']) == (pf[i]['u'], pf[i]['v'])]
    print(f'  frames 0..{n - 1}: {len(ident)} identical in pixels and size, {len(hot)} with the same hotspot')
    if len(ident) != n:
        print('  differ: ' + ' '.join(str(i) for i in range(n) if i not in ident)[:200])
    return 0


def stage(root, outdir, names=None):
    folder = os.path.join(outdir, 'SATURN')
    os.makedirs(folder, exist_ok=True)
    made = 0
    for n in names or STAGE:
        frames, pal, _ = read_scs(os.path.join(root, 'SPRITES', n + '.SCS'))
        frames = frames[:LIMIT.get(n, len(frames))]
        psx, spr = convert(frames, pal)
        open(os.path.join(folder, n + '.PSX'), 'wb').write(psx)
        open(os.path.join(folder, n + '.SPR'), 'wb').write(spr)
        made += 1
    print(f'{folder}: {made} Saturn sprites converted')
    return 0


def main():
    a = sys.argv[1:]
    if len(a) == 3 and not a[0].startswith('--'):
        frames, pal, _ = read_scs(a[0])
        psx, spr = convert(frames, pal)
        open(a[1], 'wb').write(psx)
        open(a[2], 'wb').write(spr)
        return 0
    if len(a) == 3 and a[0] == '--stage':
        return stage(a[1], a[2])
    if len(a) == 3 and a[0] == '--stage-all':
        return stage(a[1], a[2], ALL_EXPLOSIONS + STAGE)
    if len(a) == 4 and a[0] == '--compare':
        return compare(a[1], a[2], a[3])
    sys.exit(__doc__)


if __name__ == '__main__':
    sys.exit(main())
