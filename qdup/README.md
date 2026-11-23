# qdup: QD Up

QD Up, the unreleased 1996 pool game by NMS Software.

- **`POOL2/`:** NMS's source from `P_200996.ZIP` (19 September 1996). One codebase for PS1 (`#ifdef PSX`), Saturn and DOS. Every restoration change is marked `RESTORE:`, and [docs/DECISIONS.md](../docs/DECISIONS.md) explains each one.
- **`build.sh`:** builds the game against the Psy-Q layer (`psyq/`). Modes:
  - `devkit` (default): 8 MB, files over the dev kit's PC link (`--pcdrv`)
  - `retail`: 2 MB, files over the PC link
  - `cd`: 2 MB, files from the CD
  - one build holds both views: NMS's 2D view and the PS1 3D table view (`escher/`, written for the restoration, not NMS's). The player picks the view on a small screen after START on the option screen (`new/viewchoice.c`), and L2+R2 together switch it during the game.
- **`disc.sh`:** builds the bootable CD image, `build/qdup-cd/qdup.cue`, with both views. It needs the game data from the original archive (`P_200996.ZIP`'s `WORK/SONY/POOL2` folder, unpacked by `tools/originals.sh`, or `QDUP_DATA`), which is not in this repository. The disc holds `SYSTEM.CNF`, `PSX.EXE` and `QDUP.DAT`, an index and the game's files (`tools/qdup_dat.py`; the root directory is one sector, which holds about forty names, so the files go in one file and `FILE_IO.C` reads it as 360X's loader reads `360X.DAT`).
- **`-DQDUP_CRYPT_SIZE=0|1`** (in `QDUP_CFLAGS`): a test build in which the option screen's Crypt table is played, with the US (0) or UK (1) size. Without it the Crypt choice plays the Potters table: its size is not settled (docs/DECISIONS.md).

Option screen: the table icon chooses Potters (the first two models; US or UK size follows the game type), Crypt or Square (the Square plays at the US size); the baize icon (second page, second icon) blue, green or red (3D build); the balls icon UK pool, 9-ball or US 8-ball.

Pad keys in the game, beyond NMS's (`PAD.C`): Start pauses (NMS's P), L2 undoes the last shot (U), R2 replays it (R) and skips to the end of a shot while the balls roll (Z). In the 3D build L2 and R2 together toggle the 2D view. Trick-shot playback (ESC) is not reachable: no shot files exist.

Notes on `POOL2/`:
- **Physics:** `FIXBALL.C`, `FIXBALL.H`, `FIXMATH.C` and `FIXTAB.H` are the fixed-point physics the game runs. `FIXBALL.C` is NMS's own unfinished port, finished in their style. `BALL.C`, NMS's double-precision original, stays in the folder for reference and is not built.
- **Saturn and DOS code:** the Saturn and DOS branches stay as NMS wrote them, but cannot be built from this repository: the SDK headers they include belong to other companies and are left out.
- **Compiling:** NMS's files end with a DOS end-of-file byte and include each other in lower case. The build compiles cleaned copies and leaves the sources as they are.
