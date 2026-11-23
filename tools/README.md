# tools

Python scripts used for the restoration. They run on the host with Python 3. The image scripts need Pillow, and `qdup_esh_view.py` and `x360_match.py` also need NumPy; the rest use only the standard library (and the MIPS toolchain where they read or build executables).

## Build helpers

| Script | What it does |
|---|---|
| `originals.sh` | Unpacks NMS's archives (`360X.ZIP`, `20SEPT96.ZIP`, `P_200996.ZIP`) into `build/originals`, where the build scripts and tools look for the data by default. Checks each archive's sha256 and repairs five known bit flips in `P_200996.ZIP` first |
| `sdk.sh` | Sourced by the build scripts to find the PSoXide SDK: `PSOXIDE_REPO` if set, otherwise a clone in `build/psoxide-full` checked out at the commit `psyq/build.sh` pins |
| `elf2psexe.py` | Packs a linked MIPS ELF into a PS-X EXE (used by `psoxide-c/build.sh`) |
| `asmpsx2gas/` (Rust) | Rewrites NMS's assembler (SN asmpsx syntax) as GNU `as` source, instruction for instruction; the builds compile it (`cargo build` into `build/asmpsx2gas`) |
| `asm_check.py` | Checks the rewritten assembler against NMS's 1996 builds, word for word |
| `x360_film.py` | Makes 360X's intro film: NMS's `SAT2.FLC` decoded with ffmpeg and encoded with the SDK's psx-str-cook as a video-only STR for the SDK's FMV player, and the C header with its measured size |
| `x360_scs2spr.py` | Converts the Saturn port's `SPRITES/*.SCS` sprite files to the `.PSX` image and `.SPR` frame table the 360X loader reads (`--stage` for the ones the game uses, `--stage-all` for every explosion, `--compare` against NMS's PS1 art); needs only the standard library |
| `x360_dat.py` | Makes 360X's `360X.DAT`, the indexed file NMS's CD loader reads (format notes inside) |
| `qdup_gen_tables.py` | Generates `qdup/POOL2/FIXTAB.H`, the arctangent table behind the fixed-point physics |

## Reading NMS's 1996 builds

| Script | What it does |
|---|---|
| `cpe_dis.py` | Disassembles functions of a dev kit executable (`.CPE`) by name |
| `sn_sym.py` | Reads SN Systems `.SYM` debugger symbol files |
| `qdup_esh_view.py` | Renders a reference still of QD Up's 3D table from NMS's Escher model files (ESH, EMP) |

## Running and comparing

| Script | What it does |
|---|---|
| `qdup_run.py` | Runs QD Up headless (1996 build, rebuild or disc) and logs game variables per frame |
| `x360_run.py` | The same for 360X |
| `x360_match.py` | Compares two 360X runs frame by frame at matched game states |
| `x360_sound.py` | Logs what 360X's disc build plays, key-on by key-on (screen, voice, volume, pitch, bank, program, note), with the audio to a WAV file only; checks the mission music against its SEQ file |
| `shots_side_by_side.py` | Puts two runs' screenshots side by side |

## QD Up physics checks

| Script | What it does |
|---|---|
| `qdup_oracle.py` | Records ball-physics reference shots from NMS's 1996 build: start state from RAM, then per-frame ball tracks |
| `qdup_ref.py` | A host transcription of NMS's double-precision physics (`BALL.C`), used to check the recordings |
| `qdup_refcheck.py` | Checks that transcription against a recording |
| `qdup_gen_state.py` | Turns recorded start states into C for `tests/qdup-physics` |
| `qdup_compare.py` | Runs the physics test on recorded shots and compares it with the 1996 build |
| `qdup_cost96.py` | Estimates what the 1996 double-precision physics costs per call |

Most scripts expect the PSoXide emulator and the original game data. The data defaults to `build/originals` (run `originals.sh` first); `X360_DATA`, `X360_SATURN` and `QDUP_DATA` override it. The emulator is found through `PSOXIDE_EMU`, and `PSOXIDE_EMU_DEVKIT` names a build with the `devkit-ram` feature for the 1996 builds that expect the dev kit's 8 MB.
