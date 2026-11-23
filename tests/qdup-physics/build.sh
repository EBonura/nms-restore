#!/bin/sh
# Build the QD Up physics test, one PS-X EXE per recorded shot and variant:
#   build/qdup-physics/<shot>_orig.exe   tables as in the 1996 build's RAM,
#                                        1996 cushion logic (no extra-step clamp)
#   build/qdup-physics/<shot>_fixed.exe  tables from the text file, clamp on
# Extra arguments go to the compiler (e.g. -O0).
set -e
here=$(cd "$(dirname "$0")" && pwd)
root=$here/../..
pool=$root/qdup/POOL2
out=${QDUP_OUT:-$root/build/qdup-physics}
src=$out/src
mkdir -p "$src"
# NMS's files end with a DOS end-of-file byte (^Z) that GCC rejects, and
# include each other by lower-case names: compile cleaned lower-case copies.
for f in TYPES.H ESCHER.H FIXBALL.H FIXTAB.H FIXBALL.C FIXMATH.C; do
	tr -d '\032' < "$pool/$f" > "$src/$(echo $f | tr A-Z a-z)"
done
# NMS's slSin and slCos (SGL.S, table SIN_COS.TAB), as the game links them
tr -d '\032' < "$pool/SGL.S" > "$src/sgl.asm"
cp "$pool/SIN_COS.TAB" "$src/sin_cos.tab"
asmpsx2gas="$root/build/asmpsx2gas/release/asmpsx2gas"
cargo build -q --release --manifest-path "$root/tools/asmpsx2gas/Cargo.toml" --target-dir "$root/build/asmpsx2gas"
"$asmpsx2gas" "$src/sgl.asm" "$src/sgl.s"
for s in "$here"/gen/shot_*.c; do
	shot=$(basename "$s" .c); shot=${shot#shot_}
	for v in orig fixed; do
		if [ $v = orig ]; then csh=csh_ram; def=-DFIXBALL_NO_EXTRA_CLAMP; else csh=csh_text; def=; fi
		"$root/psoxide-c/build.sh" "$out/${shot}_$v.exe" -DFIXBALL_STANDALONE -I"$src" -I"$here" -Wall -Wno-comment \
			-Wno-unused-variable -Wno-unused-but-set-variable -Wno-maybe-uninitialized $def "$@" -- \
			-Wa,-I"$src" "$src/fixball.c" "$src/fixmath.c" "$src/sgl.s" \
			"$here/main.c" "$here/stubs.c" "$here/gen/$csh.c" "$s"
	done
done
echo "built $(ls "$out"/*.exe | wc -l | tr -d " ") test programs in $out"
