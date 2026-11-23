#!/bin/sh
# Build the libm check against the Psy-Q layer: build/psyq-libm/libm.exe
set -e
here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/../.." && pwd)
out=$root/build/psyq-libm
mkdir -p "$out"
PSYQ_DEBUG=1 "$root/psyq/build.sh"
mipsel-none-elf-gcc -march=r3000 -mabi=32 -msoft-float -mno-abicalls -fno-pic -G0 -O0 \
	-ffreestanding -fno-builtin -nostdinc -isystem "$(mipsel-none-elf-gcc -print-file-name=include)" \
	-I"$root/psyq/include" -Dmain=nms_main -c "$here/main.c" -o "$out/main.o"
mipsel-none-elf-ld -T "$root/psyq/psyq.ld" -u _start -o "$out/libm.elf" "$out/main.o" "$root/build/psyq/libpsyq.a"
mipsel-none-elf-objcopy -O binary -j .ps_exe_head -j .text -j .data "$out/libm.elf" "$out/libm.exe"
size=$(wc -c < "$out/libm.exe" | tr -d ' ')
pad=$(( (2048 - size % 2048) % 2048 ))
[ $pad = 0 ] || head -c $pad /dev/zero >> "$out/libm.exe"
