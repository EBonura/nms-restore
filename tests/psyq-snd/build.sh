#!/bin/sh
# Build the libsnd checks against the Psy-Q layer: build/psyq-snd/snd.exe
# (one SsUtKeyOn note) and build/psyq-snd/seq.exe (a SEQ song)
set -e
here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/../.." && pwd)
out=$root/build/psyq-snd
mkdir -p "$out"
PSYQ_DEBUG=1 "$root/psyq/build.sh"
for t in snd:main seq:seq; do
	name=${t%%:*}
	src=${t##*:}
	mipsel-none-elf-gcc -march=r3000 -mabi=32 -msoft-float -mno-abicalls -fno-pic -G0 -O0 \
		-ffreestanding -fno-builtin -nostdinc -isystem "$(mipsel-none-elf-gcc -print-file-name=include)" \
		-I"$root/psyq/include" -Dmain=nms_main -c "$here/$src.c" -o "$out/$src.o"
	mipsel-none-elf-ld -T "$root/psyq/psyq.ld" -u _start -o "$out/$name.elf" "$out/$src.o" "$root/build/psyq/libpsyq.a"
	mipsel-none-elf-objcopy -O binary -j .ps_exe_head -j .text -j .data "$out/$name.elf" "$out/$name.exe"
	size=$(wc -c < "$out/$name.exe" | tr -d ' ')
	pad=$(( (2048 - size % 2048) % 2048 ))
	[ $pad = 0 ] || head -c $pad /dev/zero >> "$out/$name.exe"
	echo "built $out/$name.exe"
done
