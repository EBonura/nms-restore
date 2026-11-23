#!/bin/sh
# Build a PS-X EXE from C sources with the PSoXide C runtime.
#   psoxide-c/build.sh OUT.exe [CFLAGS...] -- SOURCES...
set -e
here=$(cd "$(dirname "$0")" && pwd)
out=$1; shift
cflags=""
while [ "$1" != "--" ]; do cflags="$cflags $1"; shift; done
shift
obj=$(mktemp -d)
mipsel-none-elf-gcc -march=r3000 -mabi=32 -msoft-float -mno-abicalls -fno-pic -G8 -O2 -ffreestanding -fno-builtin \
  -nostdlib -DPSOXIDE_TEST_RUNTIME -I"$here" $cflags -T "$here/psx.ld" -o "$obj/out.elf" \
  "$here/crt0.S" "$here/tty.c" "$here/string.c" "$@" -lgcc
python3 -I "$here/../tools/elf2psexe.py" "$obj/out.elf" "$out"
cp "$obj/out.elf" "${out%.exe}.elf"
rm -rf "$obj"
