#!/bin/sh
# Build QD Up's disc: build/qdup-cd/qdup.bin + qdup.cue (raw 2352-byte
# sectors, one data track), bootable in the PSoXide emulator without --pcdrv.
#
#   qdup/disc.sh [DATA]      DATA: the 1996 data folder (POOL2 of P_200996)
#
# The game is built with `qdup/build.sh cd` (files read from the CD through
# the Psy-Q layer's libcd). The disc holds SYSTEM.CNF, PSX.EXE and QDUP.DAT,
# which tools/qdup_dat.py makes from the files the game opens (the format and
# the reason, a one-sector root directory, are in its header; FILE_IO.C
# LoadFile reads it as 360X's loader reads 360X.DAT). The list is what a dev
# kit run opens with PSOXIDE_PCDRV_TRACE=1 (title, option screen, Potters
# table, a shot, a computer turn), plus the other tables the table icon
# chooses: the Potters UK table (POTTERUK.ESH, its collision data UKCOLIS.BMP,
# POTTERUK.EMP in the 3D build), the Square (SQUARE.ESH, SQRCOLIS.BMP,
# SQUARE.EMP) and the Crypt (TABLES/CRYPT: COFFIN2.ESH, COFFCOLI.BMP,
# COFFIN2.EMP, BRIGHT.TAB, GRRR.PAL, POOLPAL.TAB), and, in the 3D build, the
# green and red baize palettes (POTTGREN.PAL, POTTRED.PAL, GRENPAL.TAB,
# REDPAL.TAB). //POTMEN//char01.raw is opened too but is not in the 1996
# data; the game carries on without it, from the disc as over the PC link.
#
# One disc holds both views (qdup/build.sh), so it carries the 3D view's map
# data, brightness and palette tables too.
#
# The image is made by the PSoXide SDK's mkisopsx (PSoXide/tools/mkisopsx),
# built here into build/mkiso-target from the checkout tools/sdk.sh picks.
set -e
# NMS's files are not UTF-8: sed and tr work on them byte by byte only in the C locale
export LC_ALL=C
here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/.." && pwd)
data=${1:-${QDUP_DATA:-$root/build/originals/qdup/WORK/SONY/POOL2}}
. "$root/tools/sdk.sh"
# QDUP_OUT: the output folder (a test build's; a path from the repository root), build/qdup-cd by default
out=${QDUP_OUT:-build/qdup-cd}
case $out in /*) ;; *) out=$root/$out ;; esac

QDUP_OUT=$out "$here/build.sh" cd

# RESTORE: platform. The files go into QDUP.DAT in the order the game opens them
# (the title picture, then the option screen's graphics and models, then the first
# table's files: the one a game of UK pool on the Potters table uses), so each read
# starts near where the last one ended; the rest follow. docs/DECISIONS.md: Restoration
# decisions, QD Up.
files="GFX/FRONT.TGA"
# the option screen (MENU.C, PSX.C LoadAllWrapperGfx): backdrop, title,
# font, icon animations and the PMD models it draws
for f in BACKGRND TITLE NUMBER8 FONT REFLECT1 BALLS TABLE PLAYERS PREFS START SPEAKER BAIZE PAD CARD TROPHY; do
	files="$files GFX/$f.PSX"
done
for f in UKTABLE USTABLE COFFIN SQUARE 9BALL TRIANGLE TRICK HEAD COMPUTER SPEAKER; do
	files="$files MODELS/PMD/$f.PMD"
done
files="$files TABLES/POTTERS/POTTBLUE.PAL"
files="$files COSINE.TAB TABLES/POTTERS/POTTERUK.ESH TABLES/POTTERS/BALLSCUE.ESH"
files="$files TABLES/POTTERS/POTTERUK.EMP TABLES/POTTERS/BALLSCUE.EMP TABLES/POTTERS/BRIGHT.TAB TABLES/POTTERS/BLUEPAL.TAB"
files="$files TABLES/POTTERS/UKCOLIS.BMP TABLES/POTTERS/POTTERUS.ESH TABLES/POTTERS/USTABLE.BMP"
files="$files TABLES/POTTERS/SQUARE.ESH TABLES/POTTERS/SQRCOLIS.BMP TABLES/CRYPT/COFFIN2.ESH TABLES/CRYPT/COFFCOLI.BMP"
files="$files TABLES/POTTERS/POTTERUS.EMP TABLES/POTTERS/GRENPAL.TAB TABLES/POTTERS/REDPAL.TAB"
files="$files TABLES/POTTERS/POTTGREN.PAL TABLES/POTTERS/POTTRED.PAL"
files="$files TABLES/POTTERS/SQUARE.EMP TABLES/CRYPT/COFFIN2.EMP TABLES/CRYPT/BRIGHT.TAB TABLES/CRYPT/GRRR.PAL TABLES/CRYPT/POOLPAL.TAB"

for f in $files; do
	[ -f "$data/$f" ] || { echo "missing $data/$f"; exit 1; }
done
python3 -I "$root/tools/qdup_dat.py" "$data" "$out/QDUP.DAT" --list "$out/QDUP.DAT.txt" $files

cargo build -q --release --locked $offline --manifest-path "$psoxide/tools/mkisopsx/Cargo.toml" \
	--target-dir "$root/build/mkiso-target"
"$root/build/mkiso-target/release/mkisopsx" --exe "$out/qdup.exe" --out "$out/qdup.bin" --gate-file "$root/psx-gate.toml" ${MKISO_DIAG:+--diag} --volume QDUP --file "$out/QDUP.DAT"
