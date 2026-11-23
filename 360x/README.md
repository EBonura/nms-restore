# 360x: 360X (PS1)

360X, the unreleased 1996 gravity shooter by NMS Software, PS1 version.

- **`SOURCE/`:** NMS's source from `360X.ZIP` (newest file 23 July 1996). Every restoration change is marked `RESTORE:`, and [docs/DECISIONS.md](../docs/DECISIONS.md) explains each one.
- **`build.sh`:** builds the game against the Psy-Q layer (`psyq/`). Modes:
  - `devkit` (default): 8 MB, files over the dev kit's PC link
  - `retail`: 2 MB, files over the PC link
  - `cd`: 2 MB, files through NMS's own CD loader
- **`disc.sh`:** builds the bootable CD image, `build/360x-cd/360x.cue`, with the intro film `SAT2.STR` (NMS's `SAT2.FLC`, converted by `tools/x360_film.py`, which needs `ffmpeg` and the SDK's `psx-str-cook`, built by `disc.sh`; played by the SDK's FMV player before the language screen, Start or Cross skips it). `tools/x360_scs2spr.py` converts the Saturn port's `SPRITES/*.SCS` (explosions, laser, pod, vines) to the PS1 files the loader reads; it needs the Saturn tree (`X360_SATURN`, default `build/originals/360x-saturn`, the unpacked `20SEPT96.ZIP`). `tools/x360_dat.py` packs the game's files into `360X.DAT`, the one big file NMS's loader (`FILES.C`) reads. It needs the 1996 data tree (the unpacked `360X.ZIP`, `build/originals/360x` after `tools/originals.sh`, or `X360_DATA`), which is not in this repository.

Notes on `SOURCE/`:
- **Folders not built:** `MAKEBACK/` holds NMS's map tools for the PC, and `STARS/` is a test.
- **Build settings:** the game is built at `-O2` with `-fno-toplevel-reorder` (three font tables share one end marker) and `-funsigned-char` (Sony's 1996 compiler treated `char` as unsigned).

The later Saturn port of the same game is in `360x-saturn/`.
