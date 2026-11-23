#!/usr/bin/env python3
"""Put the emulator's route screenshots of two runs side by side.

    shots_side_by_side.py OUT.png DIR_A DIR_B[@SHIFT] TICK [TICK...]

One row per tick: DIR_A's tick-NNNNNN.ppm on the left, DIR_B's on the right
(at TICK - SHIFT when a shift is given, for runs whose events happen at
different ticks), each scaled to 640x512 (x2 for 320x256; 640-wide frames,
such as 360X's 640x240, are only doubled in height), the ticks written
above. Missing frames are left blank.
"""
import os
import sys

from PIL import Image, ImageDraw


def main():
    out, a, b = sys.argv[1:4]
    shift = 0
    if '@' in b:
        b, shift = b.rsplit('@', 1)
        shift = int(shift)
    ticks = [int(t) for t in sys.argv[4:]]
    w, h, label, gap = 640, 512, 14, 8
    sheet = Image.new('RGB', (2 * w + gap, len(ticks) * (h + label)), (40, 40, 40))
    draw = ImageDraw.Draw(sheet)
    for row, t in enumerate(ticks):
        y = row * (h + label)
        draw.text((4, y + 1), f'{os.path.basename(a.rstrip("/"))} tick {t} (left)   '
                  f'{os.path.basename(b.rstrip("/"))} tick {t - shift} (right)', fill=(255, 255, 255))
        for col, (d, tt) in enumerate(((a, t), (b, t - shift))):
            path = os.path.join(d, f'tick-{tt:06d}.ppm')
            if os.path.exists(path):
                im = Image.open(path).convert('RGB')
                im = im.resize((w, h), Image.NEAREST)
                sheet.paste(im, (col * (w + gap), y + label))
    sheet.save(out)


if __name__ == '__main__':
    main()
