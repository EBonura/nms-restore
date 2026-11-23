# Launcher art

The pictures the PSoXide demo disc launcher shows for 360X and QD Up: the front
cover and the two back-cover shots of each, listed in `manifest.txt`. Nothing
here is baked. The demo disc build bakes these pictures itself, so there are no
blobs to go stale.

The demo disc build picks them up when `LAUNCHER_ART` points at this directory:

    make launcher PRIVATE=1 LAUNCHER_ART=/path/to/this/repo/launcher-art

Without it a Private launcher still builds, and these two games get the plain cover.

Pictures are 4:3 except as noted. The baker scales to 104x78, 68x51, 52x39 and
64x48 without cropping, and stretches anything that is not 4:3. `crop.py` cuts
them from the full captures:

    python3 -I crop.py <captures-dir with 360x/ and qdup/> .

The 360X front is cut tight on the "360" lettering and the lead ship so it reads
at the 52x39 size. QD Up's front is the whole portrait title (320x390), which the
baker stretches wide to fill the cover (the one picture here that is not 4:3).
The demo disc's `menu-art ext <out> launcher-art` shows the baked result.
