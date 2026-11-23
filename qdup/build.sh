#!/bin/sh
# Build QD Up (qdup/POOL2, NMS's September 1996 source) against the Psy-Q
# compatibility layer (psyq/) into a PS-X EXE:
#   build/qdup/qdup.exe   (and qdup.elf, qdup.map)
#
#   qdup/build.sh [devkit|retail|cd]   default devkit: linked for 8 MB of RAM
#
#   devkit  8 MB, files over the dev kit's PC link (run with --pcdrv DIR)
#   retail  2 MB, files over the PC link
#   cd      2 MB, files from the CD (CDMASTER=1, FINAL: no .csh written);
#           qdup/disc.sh makes the disc image
# QDUP_CFLAGS adds compiler flags (after the defaults), QDUP_OUT picks the
# output folder.
#
# One build holds both views: NMS's own 2D view and the 3D table view, the PS1
# renderer written for the restoration (escher/, not NMS's), which replaces
# ENGINE.C's Escher stubs. The player picks the view after START
# (new/viewchoice.c) and can switch with L2+R2 during the game.
#
# NMS's files are compiled from cleaned copies in build/qdup/src: the DOS
# end-of-file byte (^Z) removed, file names in lower case (the sources include
# each other that way), and DOS backslashes in #include lines turned into
# slashes. NMS's assembler (CODE.S, ESCHER.S, SGL.S) is written for SN's
# asmpsx; tools/asmpsx2gas (Rust) rewrites it into GNU syntax, instruction for
# instruction. Nothing else about the sources is changed here.
set -e
# NMS's files are not UTF-8: sed and tr work on them byte by byte only in the C locale
export LC_ALL=C
here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/.." && pwd)
pool=$here/POOL2
ram=${1:-devkit}
out=${QDUP_OUT:-$root/build/qdup}
src=$out/src
obj=$out/obj
mkdir -p "$src" "$obj"

# The game's object list, MAKEFILE.MAK's OBJS in order, with NMS's fixed-point
# physics (FIXBALL.C, finished in the restoration, and its maths FIXMATH.C)
# in place of BALL.C's doubles.
cfiles="pool gfx file_io game my3ds gametype fixball fixmath rules stv3d ai scrdrws menu ram models psx common engine pad sounds link resource"
# escher/'s renderer (in libpsyq.a) in place of ENGINE.C
cfiles=$(echo "$cfiles" | sed 's/ engine / /')
sfiles="code escher sgl"

for f in "$pool"/*.C "$pool"/*.H "$pool"/*.S "$pool"/*.LST "$pool"/*.TAB; do
	b=$(basename "$f" | tr A-Z a-z)
	case $b in
		*.tab) cp "$f" "$src/$b.tmp" ;;	# binary tables, included as they are
		*) tr -d '\032' < "$f" | sed -e '/^[[:space:]]*#[[:space:]]*include/s:\\:/:g' > "$src/$b.tmp" ;;
	esac
	cmp -s "$src/$b.tmp" "$src/$b" 2>/dev/null && rm "$src/$b.tmp" || mv "$src/$b.tmp" "$src/$b"
done

gccinc=$(mipsel-none-elf-gcc -print-file-name=include)
# -O0 as NMS's MAKEFILE.MAK. At -O2 the game jumps to 0x61 while setting up the
# table (init_3d), not yet explained (psyq/README.md, open problems).
CFLAGS="-march=r3000 -mabi=32 -msoft-float -mno-abicalls -fno-pic -G0 -O0 -g
	-std=gnu89 -fpermissive -ffreestanding -fno-builtin -fcommon -fno-strict-aliasing -fwrapv -fsigned-char
	-nostdinc -isystem $gccinc -I$src -I$root/psyq/include -DPSX -Dmain=nms_main
	-Wno-comment -Wno-unused -Wno-parentheses"

case $ram in
	cd) CFLAGS="$CFLAGS -DCDMASTER=1 -DFINAL" ;;
esac

status=0
# The ball physics (FIXBALL.C, FIXMATH.C: integer only, finished in the
# restoration) is compiled at -O2, as the recorded-shot tests in
# tests/qdup-physics build it, so the game runs the code they check; at -O0
# the break and the computer's trial shots cost about three times as much.
for f in $cfiles; do
	opt=
	case $f in fixball|fixmath) opt=-O2 ;; esac
	if ! mipsel-none-elf-gcc $CFLAGS $opt $QDUP_CFLAGS -c "$src/$f.c" -o "$obj/$f.o" 2> "$obj/$f.err"; then
		echo "FAILED: $f.c ($(grep -c 'error' "$obj/$f.err") errors, $obj/$f.err)"
		status=1
	fi
done
# Restoration work in C that is not NMS's (qdup/new): the game's flags, at -O2
newfiles=""
for f in "$here"/new/*.c; do
	b=$(basename "$f" .c)
	if ! mipsel-none-elf-gcc $CFLAGS -O2 $QDUP_CFLAGS -c "$f" -o "$obj/new_$b.o" 2> "$obj/new_$b.err"; then
		echo "FAILED: new/$b.c ($obj/new_$b.err)"
		status=1
	fi
	newfiles="$newfiles $obj/new_$b.o"
done
[ $status = 0 ] || exit 1

asmpsx2gas="$root/build/asmpsx2gas/release/asmpsx2gas"
cargo build -q --release --manifest-path "$root/tools/asmpsx2gas/Cargo.toml" --target-dir "$root/build/asmpsx2gas"
for f in $sfiles; do
	"$asmpsx2gas" "$src/$f.s" "$obj/$f.gas.s"
	mipsel-none-elf-as -march=r3000 -mabi=32 -msoft-float -G0 -I"$src" -o "$obj/$f.o" "$obj/$f.gas.s"
done

# The layer: one static library with the runtime, the Psy-Q API, libc and libm, and escher/'s renderer (a crate
# psyq depends on; RESTORE: PLATFORM, so each SDK crate is linked once).
"$root/psyq/build.sh"

objs=""
for f in $cfiles $sfiles; do objs="$objs $obj/$f.o"; done
objs="$objs $newfiles"
case $ram in
	devkit) ramsize=0x800000 ;;
	retail|cd) ramsize=0x200000 ;;
	*) echo "unknown RAM size $ram"; exit 1 ;;
esac
link() {	# link [more ld arguments]
	"$root/psyq/lld.sh" --defsym=RAM_SIZE=$ramsize "$@" -T "$root/psyq/psyq.ld" --gc-sections -Map "$out/qdup.map" \
		-u _start -o "$out/qdup.elf" $objs $libs "$root/build/psyq${PSYQ_TEST_CD_FAULT:+-tcd}/libpsyq.a"
}
sym() { mipsel-none-elf-nm "$out/qdup.elf" | awk -v s="$1" '$3==s {print "0x" $1}'; }
case $ram in
	cd)
		# RESTORE: PLATFORM. The boot splash is boot-only memory (psyq/psyq-boot.ld): it runs first thing in
		# main, before grab_available_ram_for_us takes the whole heap and clears it, so the heap block gets its
		# memory back. A first link finds where .data and .bss end; the second puts the splash there.
		# psx_rt's settle_channel (out of line since SDK a222c8613) is called once per DMA channel by
		# set_entry_state, at start-up, before main: boot-only too (the font code is not: QD Up calls FntLoad).
		sed 's|/\* BOOT_EXTRA \*/|*(.text.*settle_channel*)|' "$root/psyq/psyq-boot.ld" > "$out/psyq-boot.ld"
		boot="-u __boot_entry -T $out/psyq-boot.ld"
		link $boot --defsym=BOOT_VMA=0x80180000 --defsym=BOOT_LMA=0x80180000
		vma=$(sym __bss_end); lma=$(sym __data_end)
		link $boot --defsym=BOOT_VMA=$vma --defsym=BOOT_LMA=$lma
		[ "$(sym __bss_end)" = "$vma" ] && [ "$(sym __data_end)" = "$lma" ] || { echo "boot-only link: .data or .bss moved between the two links"; exit 1; }
		bootsec="-j .boot"
		"$root/psyq/check-boot.sh" "$out/qdup.elf" PsxSplash $(mipsel-none-elf-nm "$out/qdup.elf" | awk '$3 ~ /settle_channel/ {print $3}') || { echo "boot-only link: the code above would run after the heap took its memory back"; exit 1; } ;;
	*) link; bootsec="" ;;
esac
mipsel-none-elf-objcopy -O binary -j .ps_exe_head -j .text -j .data $bootsec "$out/qdup.elf" "$out/qdup.exe"
# whole 800h-byte sectors, as the header's size says
size=$(wc -c < "$out/qdup.exe" | tr -d ' ')
pad=$(( (2048 - size % 2048) % 2048 ))
[ $pad = 0 ] || head -c $pad /dev/zero >> "$out/qdup.exe"
# The layer is built with every delay-slot filler search on (psyq/build.sh): reroute what it leaves, prove none remains.
"$root/psyq/hazard.sh" "$out/qdup.exe" "$out/qdup.map"
# RESTORE: PLATFORM. A disc's executable carries the gate's receipt (sdk/docs/GATE.md): psx-gate over the
# layer, then the stamp, as the last change to the file.
[ "$ram" = cd ] && "$root/psyq/stamp.sh" "$out/qdup.exe"
echo "built $out/qdup.exe ($ram, $(wc -c < "$out/qdup.exe" | tr -d ' ') bytes)"
