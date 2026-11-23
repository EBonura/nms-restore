"""Crop the 360X and QD Up captures to the 4:3 pictures the demo disc launcher bakes.

The baker resizes every picture to a 4:3 box, so a capture that is not 4:3
(or is a 640x240 hi-res frame, which is 4:3 once its pixels are doubled
across) is cut here first. Only crops and one black margin; nothing is drawn.

Usage: python3 -I crop.py <frames-dir> <output dir (this directory)>
"""
import sys
from pathlib import Path
from PIL import Image

src, out = Path(sys.argv[1]), Path(sys.argv[2])


def crop(name, dst, box):
    Image.open(src / name).convert("RGB").crop(box).save(out / dst, optimize=True)


# 640x240 hi-res: 4:3 on screen means 8:3 in pixels. Tight on the "360" lettering
# and the lead ship so the logo still reads at the 52x39 size (one crop serves all
# three sizes).
crop("360x/title.png", "360x-title.png", (60, 60, 380, 180))
crop("360x/play-m1-explosion-a.png", "360x-explosion.png", (0, 0, 320, 240))
crop("360x/planet-select.png", "360x-planet.png", (0, 0, 640, 240))

# 320x480 portrait: the whole title, cut plain. It is not 4:3 (320x390), and the
# baker stretches it to fill the cover, which suits the tall lettering and rack.
crop("qdup/title.png", "qdup-title.png", (0, 10, 320, 400))
# 320x256 play frames: drop the help text rows, keep the table at 4:3.
for name, dst in (("play-3d-table-start.png", "qdup-table.png"), ("play-3d-cue-shoot.png", "qdup-cue.png")):
    crop("qdup/" + name, dst, (21, 48, 298, 256))
