# 360x-saturn: 360X (Sega Saturn port)

NMS's Sega Saturn port of 360X, from `20SEPT96.ZIP` (19 September 1996), as found. It is code and build files only: NMS's C, SH-2 assembler (`SHCODE/`) and Saturn hardware code (`VDP/`, `BUPRAM/`, `PAD/`).

This repository does not build it. It is kept because it is two months later than the PS1 tree, so it shows NMS's later intent. The restoration uses it as evidence for the PS1 version: the full credits list, the high score save in `BUPRAM/BUPRAM.C`, and the to-do notes in `SCHED/SCHEDUL.TXT`.

Left out:
- `SHCODE/MATHS.S`, marked "(c) 1996 Core Design LTD."
- `SHCODE/QMEMCPY.S` and `SHCODE/QMEMSET.S`, SH-2 memory routines by Toshiyasu Morita, whose licence is not stated
- A phone number in `SNSYSTEM/README.TXT`
- The built executable, objects, symbol and map files, and the sound driver binary, which contain Sega's library code
- The game data, which stays in the original archive
- The Sega library headers the code includes, which were never in the archive
