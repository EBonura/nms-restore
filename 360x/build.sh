#!/bin/sh
# Build 360X (360x/SOURCE, NMS's July 1996 source) against the Psy-Q
# compatibility layer (psyq/) into a PS-X EXE:
#   build/360x/360x.exe   (and 360x.elf, 360x.map)
#
#   360x/build.sh [devkit|retail|cd]   default devkit: linked for 8 MB of RAM
#
#   devkit  8 MB, files over the dev kit's PC link (run with --pcdrv DIR)
#   retail  2 MB, files over the PC link
#   cd      2 MB, files from the CD (MAIN.C's cdmaster=YES: FILES.C reads
#           them through the 360X.DAT index); 360x/disc.sh makes the disc
# X360_CFLAGS adds compiler flags (after the defaults), X360_OUT picks the
# output folder, X360_DATA the 1996 tree whose SOURCE/*.BIN tables and
# GRAPHICS/MISC/FONTS/FONTS.TIM INCBINS.S includes (assets stay out of the
# repository).
#
# NMS's files are compiled from cleaned copies in build/360x/src: the DOS
# end-of-file byte (^Z) removed and DOS backslashes in #include and incbin
# lines turned into slashes. INCBINS.S is SN asmpsx source; tools/asmpsx2gas
# rewrites it into GNU syntax. The CD functions come from the layer (psyq/src/cd.rs);
# with cdmaster=NO (devkit, retail) the game loads over the PC link and does
# not call them.
set -e
# NMS's files are not UTF-8: sed and tr work on them byte by byte only in the C locale
export LC_ALL=C
here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/.." && pwd)
srcdir=$here/SOURCE
ram=${1:-devkit}
out=${X360_OUT:-$root/build/360x}
data=${X360_DATA:-$root/build/originals/360x}
src=$out/src
obj=$out/obj
mkdir -p "$src" "$obj"

# MAKEFILE.MAK's object list in OBJLIST.CMD order, then main.c.
cfiles="FIRE PARTICLE INTERUPT SPRITE CONTROL SCROLLER LEVEL PROCESS SPRITE2 LOADDATA PANEL GENERAL FILES SOUND SPRLIST JALLOC TITLE OPTIONS LANGUAGE SHIPDEMO PLANETS HIGH CREDITS TELETYPE LOADSAVE MAIN"

for f in "$srcdir"/*.C "$srcdir"/*.H "$srcdir"/*.S "$srcdir"/*.DAT; do
	b=$(basename "$f")
	tr -d '\032' < "$f" | sed -e '/^[[:space:]]*#[[:space:]]*include/s:\\:/:g' -e '/incbin/s:\\:/:g' > "$src/$b.tmp"
	cmp -s "$src/$b.tmp" "$src/$b" 2>/dev/null && rm "$src/$b.tmp" || mv "$src/$b.tmp" "$src/$b"
done
# the pad button icons (new work, GENERAL.C Pad_Icons_Into_Fonts), from their drawing
python3 -I "$root/tools/x360_padicons.py" "$here/art/padicons.txt" "$src/padicons.h.tmp"
cmp -s "$src/padicons.h.tmp" "$src/padicons.h" 2>/dev/null && rm "$src/padicons.h.tmp" || mv "$src/padicons.h.tmp" "$src/padicons.h"
# the binaries INCBINS.S includes, from the 1996 tree
for f in SOURCE/SQRTABLE.BIN SOURCE/SINTABLE.BIN GRAPHICS/MISC/FONTS/FONTS.TIM; do
	mkdir -p "$out/$(dirname "$f")"
	cmp -s "$data/$f" "$out/$f" 2>/dev/null || cp "$data/$f" "$out/$f"
done

gccinc=$(mipsel-none-elf-gcc -print-file-name=include)
# Plain char is unsigned: the 1996 build loads 360X's BYTE (char) fields with
# lbu (sprctrl.red, .forceZ...), and forceZ's "!=128" test only works so.
# NMS's MAKEFILE.MAK compiled with -O4. -O2 here; -fno-toplevel-reorder
# keeps variables in the order they are defined, as NMS's compiler did:
# SPRLIST.C's three font sprite tables have one end marker (LASTSPR, at the
# end of the third) and Sprite_Off/Init_Spr_List walk from the first
# through all three. X360_OPT overrides (the rebuild also runs at -O0, about
# half as fast as the 1996 build); X360_O0 lists files to compile at -O0.
OPT=${X360_OPT:--O2 -fno-toplevel-reorder}
CFLAGS="-march=r3000 -mabi=32 -msoft-float -mno-abicalls -fno-pic -G0 $OPT -g
	-x c -std=gnu89 -fpermissive -ffreestanding -fno-builtin -fcommon -fno-strict-aliasing -fwrapv -funsigned-char
	-nostdinc -isystem $gccinc -I$src -I$root/psyq/include -Dmain=nms_main
	-Wno-comment -Wno-unused -Wno-parentheses"
case $ram in
	cd) CFLAGS="$CFLAGS -DCDMASTER -I$out" ;;   # film.h (tools/x360_film.py, made by disc.sh) is in $out
esac

status=0
for f in $cfiles; do
	fopt=""
	case " $X360_O0 " in *" $f "*) fopt=-O0 ;; esac
	if ! mipsel-none-elf-gcc $CFLAGS $X360_CFLAGS $fopt -c "$src/$f.C" -o "$obj/$f.o" 2> "$obj/$f.err"; then
		echo "FAILED: $f.C ($(grep -c 'error' "$obj/$f.err") errors, $obj/$f.err)"
		status=1
	fi
done
[ $status = 0 ] || exit 1

asmpsx2gas="$root/build/asmpsx2gas/release/asmpsx2gas"
cargo build -q --release --manifest-path "$root/tools/asmpsx2gas/Cargo.toml" --target-dir "$root/build/asmpsx2gas"
"$asmpsx2gas" "$src/INCBINS.S" "$obj/INCBINS.gas.s"
mipsel-none-elf-as -march=r3000 -mabi=32 -msoft-float -G0 -I"$out/SOURCE" -o "$obj/INCBINS.o" "$obj/INCBINS.gas.s"

# The layer: one static library with the runtime, the Psy-Q API, libc and libm.
"$root/psyq/build.sh"

objs="$obj/INCBINS.o"
for f in $cfiles; do objs="$objs $obj/$f.o"; done
case $ram in
	devkit) ramsize=0x800000 ;;
	# 2 MB: 16 KB kept for the stack, not the layer's 32 KB: the heap needs the
	# room (docs/DECISIONS.md, 360X's missions). The stack was measured at 1.3 KB (boot,
	# film, menus, Mission 1 with firing and pause).
	retail|cd) ramsize=0x200000; stack="--defsym=STACK_RESERVE_OVERRIDE=0x4000" ;;
	*) echo "unknown RAM size $ram"; exit 1 ;;
esac
link() {	# link [more ld arguments]
	"$root/psyq/lld.sh" --defsym=RAM_SIZE=$ramsize $stack "$@" -T "$root/psyq/psyq.ld" --gc-sections -Map "$out/360x.map" \
		-u _start -o "$out/360x.elf" $objs "$root/build/psyq${PSYQ_TEST_CD_FAULT:+-tcd}/libpsyq.a"
}
sym() { mipsel-none-elf-nm "$out/360x.elf" | awk -v s="$1" '$3==s {print "0x" $1}'; }
case $ram in
	cd)
		# The film player is boot-only memory (psyq/psyq-boot.ld): it runs from the
		# start of the heap and the heap takes its 25 KB back after the film. A first
		# link finds where .data and .bss end; the second puts it there.
		# 360X never calls FntLoad (the layer's debug font), so the font code the splash draws its line with
		# is boot-only too; a program that does call it must not have these two lines (check-boot.sh fails).
		# psx_vram::upload_16bpp (out of line since SDK 2830eea7) is called by FontAtlas::upload alone here, so it is boot-only too.
		sed 's|/\* BOOT_EXTRA \*/|*(.text.*FontAtlas6upload* .text.*FontAtlas9draw_text* .text.*psx_vram12upload_16bpp*)|' "$root/psyq/psyq-boot.ld" > "$out/psyq-boot.ld"
		boot="-u __boot_entry -T $out/psyq-boot.ld"
		link $boot --defsym=BOOT_VMA=0x80180000 --defsym=BOOT_LMA=0x80180000
		vma=$(sym __bss_end); lma=$(sym __data_end)
		link $boot --defsym=BOOT_VMA=$vma --defsym=BOOT_LMA=$lma
		[ "$(sym __bss_end)" = "$vma" ] && [ "$(sym __data_end)" = "$lma" ] || { echo "boot-only link: .data or .bss moved between the two links"; exit 1; }
		bootsec="-j .boot"
		# RESTORE: PLATFORM. The heap takes the boot-only memory back after the film and the splash:
		# nothing outside it may call into it except their entry points.
		"$root/psyq/check-boot.sh" "$out/360x.elf" FmvPlay FmvMemoryBytes PsxSplash || { echo "boot-only link: the code above would run after the heap took its memory back"; exit 1; } ;;
	*) link; bootsec="" ;;
esac
mipsel-none-elf-objcopy -O binary -j .ps_exe_head -j .text -j .data $bootsec "$out/360x.elf" "$out/360x.exe"
size=$(wc -c < "$out/360x.exe" | tr -d ' ')
pad=$(( (2048 - size % 2048) % 2048 ))
[ $pad = 0 ] || head -c $pad /dev/zero >> "$out/360x.exe"
# The layer is built with every delay-slot filler search on (psyq/build.sh): reroute what it leaves, prove none remains.
"$root/psyq/hazard.sh" "$out/360x.exe" "$out/360x.map"
# RESTORE: PLATFORM. A disc's executable carries the gate's receipt (sdk/docs/GATE.md): psx-gate over the
# layer, then the stamp, as the last change to the file.
[ "$ram" = cd ] && "$root/psyq/stamp.sh" "$out/360x.exe"
echo "built $out/360x.exe ($ram, $(wc -c < "$out/360x.exe" | tr -d ' ') bytes)"
