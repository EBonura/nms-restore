#!/bin/sh
# Build 360X's disc: build/360x-cd/360x.bin + 360x.cue (raw 2352-byte
# sectors, one data track), bootable in the PSoXide emulator without --pcdrv.
#
#   360x/disc.sh [DATA]      DATA: the 1996 tree (the unpacked 360X.ZIP); X360_SATURN: the Saturn
#                            tree (20SEPT96.ZIP), whose SPRITES/*.SCS tools/x360_scs2spr.py converts
#
# The game is built with `360x/build.sh cd` (MAIN.C's cdmaster=YES, 2 MB):
# FILES.C finds \360X.DAT;1 with CdSearchFile, reads its index and loads
# every file from sectors inside it. tools/x360_dat.py makes 360X.DAT from
# the tree's INTRO, LEVEL1, MAPS, MISC and SOUND folders (format notes there)
# and the creature files of missions 2 to 9, which tools/x360_txt2spr.py
# --stage compiles from the tree's GRAPHICS/LEVELn art into build/360x-cd/levels
# (NMS's TXT2SPR step; LOADDATA.C names the files).
# The disc holds SYSTEM.CNF, PSX.EXE, 360X.DAT and SAT2.STR (the intro film,
# tools/x360_film.py, streamed by the layer's FmvPlay) in the root directory.
#
# The image is made by the PSoXide SDK's mkisopsx (PSoXide/tools/mkisopsx),
# built here into build/mkiso-target from the checkout tools/sdk.sh picks.
set -e
# NMS's files are not UTF-8: sed and tr work on them byte by byte only in the C locale
export LC_ALL=C
here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/.." && pwd)
data=${1:-${X360_DATA:-$root/build/originals/360x}}
. "$root/tools/sdk.sh"
out=${X360_OUT:-$root/build/360x-cd}
saturn=${X360_SATURN:-$root/build/originals/360x-saturn}	# the Saturn tree (20SEPT96.ZIP), for its sprites

# The intro film, NMS's SAT2.FLC as an STR for the SDK's player: ffmpeg decodes it, the SDK's
# psx-str-cook (built from the same checkout as mkisopsx) encodes it; film.h tells the game its
# size, for the build, and SAT2.STR.cook.txt records the tool, SDK commit and hashes.
mkdir -p "$out"
# (run inside the checkout: psx-str-cook links crates that need the SDK's pinned nightly toolchain)
(cd "$psoxide" && cargo build -q --release --locked $offline -p psx-str-cook \
	--target-dir "$root/build/mkiso-target")
PSX_STR_COOK="$root/build/mkiso-target/release/psx-str-cook" \
	PSX_STR_COOK_SDK_COMMIT=$(git -C "$psoxide" rev-parse HEAD) \
	python3 -I "$root/tools/x360_film.py" "$data" "$out/SAT2.STR" "$out/film.h"

X360_OUT=$out X360_DATA=$data "$here/build.sh" cd
rm -rf "$out/levels"
python3 -I "$root/tools/x360_txt2spr.py" --stage "$data" "$here/SOURCE/LOADDATA.C" "$out/levels"
python3 -I "$root/tools/x360_scs2spr.py" --stage "$saturn" "$out/levels"
python3 -I "$root/tools/x360_dat.py" "$data" "$out/360X.DAT" --list "$out/360X.DAT.txt" --extra "$out/levels" \
	--order "$here/LOADORDER.TXT"

cargo build -q --release --locked $offline --manifest-path "$psoxide/tools/mkisopsx/Cargo.toml" \
	--target-dir "$root/build/mkiso-target"
"$root/build/mkiso-target/release/mkisopsx" --exe "$out/360x.exe" --out "$out/360x.bin" --gate-file "$root/psx-gate.toml" ${MKISO_DIAG:+--diag} --volume 360X \
	--file "$out/360X.DAT" --xa-file "$out/SAT2.STR"
