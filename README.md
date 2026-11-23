# 360X and QD Up: NMS Software restoration

360X and QD Up are two PlayStation games by NMS Software. BMG Interactive was due to publish them in 1996, but neither came out in any territory. Lead programmer Paul Griffiths kept backups of the work, and the PlayStation Museum preserved them.

This repository rebuilds both games from NMS's original C source so they run on a standard PlayStation. NMS's code stays as close to how they left it as possible. Sony's Psy-Q libraries, which the games were written against, are replaced by a clean-room compatibility layer built on [PSoXide](https://github.com/EBonura/PSoXide), an open-source PS1 SDK written in Rust. There is no Sony code, header, library or documentation in this repository.

The restored games are released together with PSX Archive.

- 360X is a gravity shooter: you fly a thrust-and-rotate craft through cave maps over nine missions.
- QD Up is a pool game with UK pool, US 8-ball and 9-ball, and three tables. The 1996 PS1 code draws a flat top-down view; the restoration adds a 3D table view drawn from NMS's own models and camera code, and the disc lets you pick either.

## Status

| | QD Up (pool) | 360X (gravity shooter) |
|---|---|---|
| Source | `qdup/POOL2`, 19 Sep 1996 | `360x/SOURCE`, 23 Jul 1996 |
| Original 1996 build | Runs in the PSoXide emulator (dev kit mode) | Runs in the PSoXide emulator (dev kit mode) |
| Rebuilt from source | Yes, no Sony code | Yes, no Sony code |
| Retail console, 2 MB | Fits; boots from a CD image | Fits; boots from a CD image; all nine missions play (chosen on the planet select) |
| Memory card | Not used by the game | High scores save and load |
| Real hardware | Runs on a PlayStation console from a burned disc | Runs on a PlayStation console from a burned disc |

NMS's full 3D pool table (the "Escher" renderer) was written for the DOS (PC CD-ROM) version, and its PS1 entry points were left empty. NMS did try a 3D table on the PS1 earlier: `GLENN/POOL.C` ("NMS - 3D - POOL, Comenced 29/04/96, Glenn Benson") draws `models/pmd/uktable2.pmd` with `RotPMD`, and `GLENN/POOL.CPE` (10 Jul) and `PPP.CPE` (4 Jun) are table viewers with no game logic. The 3D view in `escher/` is new work for the restoration, not a port of that viewer. The 2D view is NMS's own.

360X's July 1996 code had no way to finish a mission and most creatures only played their animation. The restoration completes both from NMS's own later versions (the Saturn port and the PC code in the same archives); [docs/DECISIONS.md](docs/DECISIONS.md) has the evidence for every change, mission by mission.

## Other versions: Saturn and DOS

NMS developed both games for more than one system. The archives hold the following versions.

| | QD Up | 360X |
|---|---|---|
| PS1 | `#ifdef PSX` code in `qdup/POOL2` | `360x/SOURCE`, July 1996 |
| Saturn | Same codebase, the non-PSX branch (Sega's SGL library). It draws the same flat top-down view as the PS1 | A separate port, `20SEPT96.ZIP` (19 Sep 1996): its own source tree (`VDP/`, `SHCODE/`, `SNSYSTEM/`) in `360x-saturn/`, code and build files only. The archive also has a built executable with symbols and the Saturn disc script |
| DOS (PC CD-ROM) | Same codebase, the `__PC_H` branch. It is the only version written for the full 3D table view: the "Escher" renderer that `STV3D.C` calls. No DOS build of QD Up and no Escher renderer source survive. `GAME.CCC` and `BALL.CCC` in the same folder are the PC CD-ROM version's `GAME.C` and `BALL.C` (3 Sep 1996, header "QD-UP PC-CD ROM"): they enable what the PS1 port commented out (replay, some sample calls, CD track choice) and are a reference for the DOS flow. (`PINDOS.EXE` in the same folder is a different program, "Hyper 3d Pinball".) | None found |

**Reference policy for missing features.** Where the PS1 code is unfinished and another version has the feature, the restoration rebuilds it for the PS1 with that version as the reference. That means the same data, the same behaviour and the same look, never the PS1 guessing on its own. For QD Up's 3D view the reference is the DOS code path in the shared source: the calls `STV3D.C` makes, the mesh and texture-map files (ESH, EMP), and the palette and brightness tables the PC version loaded. The PS1 renderer is written fresh on the GTE behind NMS's own `E_*` interface. Where the data left a choice open (for example the cue's colour), `escher/README.md` says so. Every such feature is marked as restoration work, not NMS's.


## Approach

Every choice made in the restoration, including what was kept as NMS left it and why, is listed in [docs/DECISIONS.md](docs/DECISIONS.md).

- **Original code first.** The first commit is the source exactly as found. Every later change to an NMS file is marked with a `RESTORE:` comment and explained in its commit message.
- **Clean-room Psy-Q layer.** `psyq/` implements the Psy-Q functions the games call, in Rust on the PSoXide SDK, with C headers written for this project. Each function was written from how NMS's code uses it, from documented hardware behaviour, and from what NMS's 1996 builds observably do in the emulator. See [`psyq/README.md`](psyq/README.md).
- **Checked against the originals.** NMS's 1996 builds run in the PSoXide emulator, which loads dev kit executables, serves files the way the dev kit's PC link did and can model the dev kit's 8 MB of RAM. Rebuilt code is compared against those builds: recorded shots, RAM state and screenshots side by side.


| The film player is boot-only memory; the heap block is 1319 KB and the stack keeps 16 KB | Platform | 2026-11-22: the block went from 1323 to 1319 KB (`MAIN.C` `maxmalloc`), 4 KB for the program: the pause screen's icons left the weapons test build 156 bytes over the block, and the SDK of that night's move adds 256 bytes, which took it 100 bytes short. Mission 1's peak still leaves 56.2 KB. Before that: The logic's data (15 KB), the wider sprite records (23 KB) and the interpreter (10 KB) come out of the same 2 MB, and so do the film player's 34 KB. Mission 1 on the disc needs 1262.8 KB of heap (1313.5 before the one-sector file rule, 1309.5 before the Saturn sprites); with everything resident and the layer's 32 KB stack reserve only 1288 KB were left and Mission 1 could not load. Two changes make room without touching the game's content. The film player's code, tables and statics (25 KB: `psx-fmv`, `FmvPlay`) are linked at the start of the heap (`psyq/psyq-boot.ld`, opt-in; `psyq/src/boot.rs` moves them there at entry), so 360X's heap block lies over them: the film plays first thing with the heap all but empty, NMS's allocator fills its block from the top down, and the last 25 KB are reached only in Mission 1, long after the film. And the stack reserve can be set at link time (`--defsym=STACK_RESERVE`): 360X's 2 MB builds keep 16 KB. QD Up links as before (32 KB, nothing boot-only). 1328 KB can be had; the block is 1324 KB. Still available if more is wanted: smaller pieces for the effects bank. The boot splash (`PsxSplash`, `psyq/src/splash.rs`, the SDK's `psx-splash`) is boot-only memory too, and runs before the film: see the splash's row under Both games. |

## Left out of the repository

- Two headers that are Sony sample code ("Copyright (C) 1993 by Sony Corporation"): `360x/SOURCE/STARS/BALLTEX.H` and `qdup/POOL2/BALLGFX.H`. Nothing in the games uses them.
- Other companies' SDK headers, used only by the DOS and Saturn branches. No PS1 build includes them:
  - SciTech UniVBE: `VESAVBE.H`, `UVBELIB.H`, `SVGA.H`
  - Miles Design AIL: `AIL.H`
  - Sega's Saturn library: `SL_DEF.H`, `SGL.H`
- From the Saturn 360X tree: `SHCODE/MATHS.S`, marked "(c) 1996 Core Design LTD."; `SHCODE/QMEMCPY.S` and `SHCODE/QMEMSET.S`, Toshiyasu Morita's SH-2 memory routines, whose licence is not stated; and the built binaries (executable, objects, the sound driver), which contain Sega's library code.

## Repository layout

| Path | Contents |
|---|---|
| `qdup/POOL2/` | QD Up source (one codebase for PS1, Saturn and DOS; see Other versions) |
| `qdup/build.sh`, `qdup/disc.sh` | QD Up executable and CD image builds ([README](qdup/README.md)) |
| `360x/SOURCE/`, `360x/build.sh`, `360x/disc.sh` | 360X PS1 source, its executable and CD image builds ([README](360x/README.md)) |
| `360x-saturn/` | 360X Saturn port source (September 1996), kept as evidence, not built ([README](360x-saturn/README.md)) |
| `psyq/` | Psy-Q compatibility layer: Rust crate, C headers, linker script ([README](psyq/README.md)) |
| `escher/` | QD Up's PS1 3D view: a Rust renderer behind NMS's Escher calls, restoration work ([README](escher/README.md)) |
| `psoxide-c/` | Minimal C runtime for small test programs ([README](psoxide-c/README.md)) |
| `tests/` | Physics tests against recorded 1996 shots, maths and sound checks, gameplay scripts ([README](tests/README.md)) |
| `tools/` | Unpacking the originals, disc packing, capture and comparison of 1996 builds ([README](tools/README.md)) |
| `launcher-art/` | Cover pictures for the PSoXide demo disc launcher ([README](launcher-art/README.md)) |
| `docs/DECISIONS.md` | Every restoration decision, with its evidence |

## Building

You need:
- `mipsel-none-elf-gcc` 14 (Homebrew: `brew install mipsel-none-elf-gcc`), Rust `nightly-2026-03-25` with `rust-src` (rustup installs it from `psyq/rust-toolchain.toml`), Python 3, `git` and `unzip`.
- `ffmpeg`, for the 360X disc only (it decodes NMS's intro film).
- NMS's original archives `360X.ZIP`, `20SEPT96.ZIP` and `P_200996.ZIP`. They hold the game data, which is not in this repository. They come from the PlayStation Museum's set of NMS's backups. An earlier, partial copy of NMS's backup CD is on archive.org ([NMS Backup CD](https://archive.org/details/nms_backup_cd)); it does not include these three archives.

Unpack the archives once (the script checks each one's sha256 and repairs five known bit flips in `P_200996.ZIP`):

```sh
tools/originals.sh /path/to/archives   # into build/originals
```

Then build the CD images:

```sh
360x/disc.sh           # build/360x-cd/360x.cue
qdup/disc.sh           # build/qdup-cd/qdup.cue, both views
```

The first build clones the PSoXide SDK at the commit `psyq/build.sh` pins into `build/`. Set `PSOXIDE_REPO` to use a checkout you already have; it has to be at the same commit for the discs to come out byte for byte the same. The architecture gate's receipt in each executable also records the repository name from `remote.origin.url`, so the discs match the released images when built from a clone of this repository. The data folders can also be given directly with `X360_DATA`, `X360_SATURN` and `QDUP_DATA`.

Executables without a disc, for the emulator's dev kit mode:

```sh
qdup/build.sh          # dev build, 8 MB, files over the PC link
qdup/build.sh retail   # 2 MB, files over the PC link
360x/build.sh          # dev build, 8 MB, files over the PC link
360x/build.sh retail   # 2 MB, files over the PC link
```

The CD images run in any PS1 emulator and on a console that can boot a burned disc. The comparison tools and tests drive the [PSoXide emulator](https://github.com/EBonura/PSoXide-emulator) headless; point `PSOXIDE_EMU` at its binary, and `PSOXIDE_EMU_DEVKIT` at a build with the `devkit-ram` feature for the 1996 builds that expect the dev kit's 8 MB.

## Licence

NMS Software's source code (`360x/SOURCE`, `360x-saturn`, `qdup/POOL2`) and the game data in the original archives remain the property of their rights holders. They are published here for preservation and study, and are not covered by the licence below.

Everything written for the restoration (`psyq/`, `escher/`, `psoxide-c/`, `tools/`, `tests/`, the build scripts and the `RESTORE:` changes) is licensed under the GNU General Public License, version 2 ([LICENSE](LICENSE)), the same licence as the PSoXide SDK it builds on.

## Credits

**360X, from the game's credits screen (Saturn version, September 1996):**
- Programming: Pratik Patel, Paul Griffiths, Phillip Trelford
- Art: Ian Margetts, Colin Dempsey
- Sound and music: The Music Bods
- Executive producer: Richard Chappells

QD Up has no credits screen in the surviving source. Its files name Glenn Benson as the author of the early 3D table viewer.

NMS's backups were preserved by the PlayStation Museum. The restored games are released together with PSX Archive.

Restoration by Bonnie Studios.
