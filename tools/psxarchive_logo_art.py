#!/usr/bin/env python3
"""Make the boot credit's logo art from PSX Archive's logo (offline, not run by the build).

    psxarchive_logo_art.py psyq/art/psxarchive-source.png psyq/art/psxarchive-logo.png

The source is the logo as PSX Archive sent it (713 by 713, on purple satin). The satin is cut
away (the purple region joined to the picture's edge; the largest shape left is the logo, its
holes filled), the logo cropped to its box, scaled to 100 rows (the height of the mark beside it), and reduced to 128 colours with
no dithering, the darkest as entry 0 (black, the screen behind it). The result is committed;
psyq/build.sh packs it with tools/psxarchive_logo.py. Needs numpy, scipy and Pillow.
"""
import sys, numpy as np
from PIL import Image
from scipy import ndimage
src = Image.open(sys.argv[1]).convert('RGB')
a = np.asarray(src).astype(int)
r, g, b = a[..., 0], a[..., 1], a[..., 2]
purple = (r > g + 8) & (b > g + 20) & (b > r)
lab, n = ndimage.label(purple)
edge = set(np.unique(np.concatenate([lab[0], lab[-1], lab[:, 0], lab[:, -1]]))) - {0}
bg = np.isin(lab, list(edge))
bg = ndimage.binary_closing(bg, iterations=2) | bg
# fill tiny islands of non-bg inside bg, and holes
fg = ~bg
fg = ndimage.binary_opening(fg, iterations=2)
lab2, n2 = ndimage.label(fg)
sizes = ndimage.sum(fg, lab2, range(1, n2 + 1))
fg = lab2 == 1 + int(np.argmax(sizes)); fg = ndimage.binary_fill_holes(fg)
out = a.copy(); out[~fg] = 0
ys, xs = np.nonzero(fg)
box = (xs.min(), ys.min(), xs.max() + 1, ys.max() + 1)
cut = Image.fromarray(out.astype(np.uint8)).crop(box)
def make(img, name, H):
    w = round(img.width * H / img.height) // 2 * 2
    sm = img.resize((w, H), Image.LANCZOS)
    q = sm.quantize(colors=128, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.NONE)
    # palette index 0 must be the background: remap so black is 0
    pal = q.getpalette()[:128*3]
    idx = np.asarray(q)
    darkest = min(range(128), key=lambda i: sum(pal[3*i:3*i+3]))
    order = [darkest] + [i for i in range(128) if i != darkest]
    remap = np.zeros(256, np.uint8)
    for new, old in enumerate(order): remap[old] = new
    newpal = sum(([*pal[3*o:3*o+3]] for o in order), [])
    out = Image.fromarray(remap[idx], 'P'); out.putpalette(newpal)
    out.save(name, optimize=False)
    print(name, out.size)
make(cut, sys.argv[2], 100)
