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
. "$root/psyq/core-window.sh"
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
	devkit) ramsize=0x800000 ;;
	# RESTORE: PLATFORM. A console's 2 MB less the SDK scene core's window and handoff block at the top
	# (psyq/core-window.sh). RAM.C's heap ends 16 KB under the stack top that gives.
	retail|cd) ramsize=$core_ram_size; CFLAGS="$CFLAGS -DPSYQ_RAM_SIZE=$ramsize" ;;
	*) echo "unknown RAM size $ram"; exit 1 ;;
esac
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
	# RESTORE: PLATFORM. The disc build's scene images (qdup/scenes.txt) take single functions of these four
	# files: their own sections for each. GAMETYPE.C's tables get their own sections too, so the option screen's
	# reads of its ball positions and palettes are told apart from the game's switch tables (tools/scenes.py
	# check). Nothing else about the code changes.
	case "$ram $f" in "cd psx"|"cd common"|"cd menu"|"cd gfx") opt="$opt -ffunction-sections" ;; esac
	case "$ram $f" in "cd gametype") opt="$opt -ffunction-sections -fdata-sections" ;; esac
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
# RESTORE: PLATFORM. The disc build splits the program into a resident base and scene images
# (qdup/scenes.txt, tools/scenes.py): their linker script goes first, each screen's entry function is
# wrapped so that a call loads its image, and the images are cut into SCENES.IMG for the disc.
scenes=""
if [ "$ram" = cd ]; then
	python3 -I "$root/tools/scenes.py" ld "$here/scenes.txt" "$out/scenes.ld" "$obj"
	python3 -I "$root/tools/scenes.py" wrap "$here/scenes.txt" "$obj/scenes_wrap.s"
	mipsel-none-elf-as -march=r3000 -mabi=32 -msoft-float -G0 -o "$obj/scenes_wrap.o" "$obj/scenes_wrap.s"
	objs="$objs $obj/scenes_wrap.o"
	scenes="-T $out/scenes.ld $(python3 -I "$root/tools/scenes.py" flags "$here/scenes.txt")"
fi
link() {	# link [more ld arguments]
	"$root/psyq/lld.sh" --defsym=RAM_SIZE=$ramsize "$@" $scenes -T "$root/psyq/psyq.ld" --gc-sections -Map "$out/qdup.map" \
		-u _start -o "$out/qdup.elf" $objs $libs "$root/build/psyq${PSYQ_TEST_CD_FAULT:+-tcd}/libpsyq.a"
}
case $ram in
	cd)
		link
		# RESTORE: PLATFORM. No code may refer into an image that may not be loaded.
		python3 -I "$root/tools/scenes.py" check "$here/scenes.txt" "$out/qdup.elf" "$out/qdup.map" $objs \
			"$root/build/psyq${PSYQ_TEST_CD_FAULT:+-tcd}/libpsyq.a"
		python3 -I "$root/tools/scenes.py" cut "$here/scenes.txt" "$out/qdup.elf" "$out" ;;
	*) link ;;
esac
mipsel-none-elf-objcopy -O binary -j .ps_exe_head -j .text -j .data "$out/qdup.elf" "$out/qdup.exe"
# whole 800h-byte sectors, as the header's size says
size=$(wc -c < "$out/qdup.exe" | tr -d ' ')
pad=$(( (2048 - size % 2048) % 2048 ))
[ $pad = 0 ] || head -c $pad /dev/zero >> "$out/qdup.exe"
# The layer is built with every delay-slot filler search on (psyq/build.sh): reroute what it leaves, prove none remains.
"$root/psyq/hazard.sh" "$out/qdup.exe" "$out/qdup.map"
if [ "$ram" = cd ]; then
	# RESTORE: PLATFORM. The images' layer code too (the splash's), through the program's trampolines; then
	# the images go into SCENES.IMG with the table.
	for f in "$out"/SCENES.*.bin; do
		"$root/psyq/hazard.sh" --image "$out/qdup.exe" "$out/qdup.map" "$f" "${f%.bin}.map"
	done
	python3 -I "$root/tools/scenes.py" pack "$here/scenes.txt" "$out" "$out/SCENES.IMG" --list "$out/SCENES.IMG.txt"
	python3 -I "$root/tools/scenes.py" splice "$here/scenes.txt" "$out" "$out/qdup.exe"
fi
# RESTORE: PLATFORM. A disc's executable carries the gate's receipt (sdk/docs/GATE.md): psx-gate over the
# layer, then the stamp, as the last change to the file.
[ "$ram" = cd ] && "$root/psyq/stamp.sh" "$out/qdup.exe"
echo "built $out/qdup.exe ($ram, $(wc -c < "$out/qdup.exe" | tr -d ' ') bytes)"
