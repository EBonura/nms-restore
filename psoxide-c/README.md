# psoxide-c: a minimal C runtime for test programs

A small runtime for building stand-alone C test programs as PS1 executables. The games do not use it: they link against the Psy-Q layer in `psyq/`, which brings the PSoXide SDK's own runtime. Here, a test can be a few C files and nothing else.

| File | What it does |
|---|---|
| `crt0.S` | Startup: sets `gp` and the stack, clears `.bss`, calls `main`, then waits in a loop |
| `psx.ld` | Linker script for a retail console: 2 MB of RAM, code from `0x80010000`, stack top at `0x801FFF00` |
| `tty.c`, `tty.h` | `tty_puts` and a small `tty_printf` (`%d %u %x %s %c`, width and zero padding) that write to the debug console at `0x1F802080`. The PSoXide emulator prints it, and `launch --stop-on-tty` can end a run on a line |
| `string.c` | `memcpy`, `memmove`, `memset`, `memcmp`, `strlen` and `strcmp`, which GCC may call by itself |
| `build.sh` | Compiles and links with `mipsel-none-elf-gcc` (R3000, soft-float, freestanding), then packs a PS-X EXE with `tools/elf2psexe.py` |

## Use

```sh
psoxide-c/build.sh build/hello.exe -- tests/hello/main.c
psoxide-emulator launch --path build/hello.exe --steps 5000000 --stop-on-tty DONE
```

Compiler flags go before `--` and sources after it. The output is `OUT.exe` plus `OUT.elf` for disassembly. The runtime has no heap, no libc beyond `string.c`, and no maths library. The physics test (`tests/qdup-physics`) uses it for that reason: its fixed-point code needs none of them.

The runtime is for test programs only: `tty.c` stores to the debug console unconditionally (a bus error on a console), so it refuses to compile unless `build.sh` has defined `PSOXIDE_TEST_RUNTIME`, and no game or disc build script names this folder.
