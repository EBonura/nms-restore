#!/usr/bin/env python3
"""Compare two 360X runs frame by frame at matched game states.

    x360_match.py RUN_A RUN_B PHASE=LO-HI[/BLO-BHI][@KEY,KEY...] [...] [--sheet OUT.png]

RUN_A and RUN_B are x360_run.py output folders made with --shots 1 and
--watch naming the key variables (default gamecycle PWorld+4 DWorld+32
craftx crafty screenx screeny). A state is the tuple of the key variables;
a frame of RUN_A at tick t is matched when RUN_B holds the same state at
some tick u (the last tick of each state is used). The frames compared
are A's at t and the closest of B's at u-3..u+3 (the screen shows the
frame drawn before, and the two builds swap at different points of a
tick). A PHASE is ticks LO-HI of RUN_A and BLO-BHI of RUN_B (default the
same), with its own key variables after @ (default the --key list). For
each phase it prints how many states
matched, how many of them are pixel-identical, and the largest and median
count of differing pixels; --sheet writes, per phase, the matched pair with
the median difference side by side with a map of the differing pixels.
"""
import argparse
import csv
import os

import numpy as np
from PIL import Image, ImageDraw

DEFAULT_KEY = ['gamecycle', 'PWorld+4', 'DWorld+32', 'craftx', 'crafty', 'screenx', 'screeny']


def states(run, key, lo=0, hi=1 << 30):
    rows = list(csv.reader(open(os.path.join(run, 'vars.csv'))))
    head, rows = rows[0], [[int(x) for x in r] for r in rows[1:]]
    cols = [head.index(k) for k in key]
    out = {}
    for i in range(1, len(rows)):
        s = tuple(rows[i][c] for c in cols)
        if lo <= rows[i][0] <= hi:
            out.setdefault(s, []).append(rows[i][0])
    return out


def frame(run, t):
    path = os.path.join(run, f'tick-{t:06d}.ppm')
    try:
        return np.array(Image.open(path).convert('RGB')).astype(int)
    except Exception:
        return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('a')
    ap.add_argument('b')
    ap.add_argument('phases', nargs='+')
    ap.add_argument('--sheet')
    ap.add_argument('--key', nargs='*', default=DEFAULT_KEY)
    a = ap.parse_args()
    rows = []
    for ph in a.phases:
        name, rng = ph.split('=')
        key = a.key
        if '@' in rng:
            rng, k = rng.split('@')
            key = k.split(',')
        ra, rb = (rng.split('/') + [rng])[:2]
        lo, hi = (int(x) for x in ra.split('-'))
        blo, bhi = (int(x) for x in rb.split('-'))
        sa, sb = states(a.a, key, lo, hi), states(a.b, key, blo, bhi)
        results = []
        for s, ticks in sa.items():
            ta = ticks[-1]
            if s not in sb:
                continue
            fa = frame(a.a, ta)
            if fa is None:
                continue
            best = None
            for u in sb[s]:
                for dt in range(-3, 4):
                    fb = frame(a.b, u + dt)
                    if fb is None or fb.shape != fa.shape:
                        continue
                    n = int((np.abs(fa - fb).max(axis=2) > 0).sum())
                    if best is None or n < best[0]:
                        best = (n, ta, u + dt)
            if best:
                results.append(best)
        results.sort()
        if not results:
            print(f'{name}: no matched states')
            continue
        same = sum(1 for r in results if r[0] == 0)
        med = results[len(results) // 2]
        print(f'{name}: {len(results)} matched states, {same} pixel-identical, '
              f'differing pixels max {results[-1][0]} median {med[0]} (of {frame(a.a, med[1]).shape[0] * frame(a.a, med[1]).shape[1]})')
        rows.append((name, med))
    if a.sheet and rows:
        w, h, label = 640, 256, 14
        sheet = Image.new('RGB', (3 * w, len(rows) * (h + label)), (40, 40, 40))
        draw = ImageDraw.Draw(sheet)
        for i, (name, (n, ta, tb)) in enumerate(rows):
            fa, fb = frame(a.a, ta), frame(a.b, tb)
            diff = (np.abs(fa - fb).max(axis=2) > 0).astype(np.uint8) * 255
            y = i * (h + label)
            draw.text((4, y + 1), f'{name}: {os.path.basename(a.a)} tick {ta} | {os.path.basename(a.b)} tick {tb} '
                      f'| differing pixels {n} (white)', fill=(255, 255, 255))
            for col, im in enumerate((Image.fromarray(fa.astype(np.uint8)), Image.fromarray(fb.astype(np.uint8)),
                                      Image.fromarray(diff).convert('RGB'))):
                sheet.paste(im.resize((w, h), Image.NEAREST), (col * w, y + label))
        sheet.save(a.sheet)


if __name__ == '__main__':
    main()
