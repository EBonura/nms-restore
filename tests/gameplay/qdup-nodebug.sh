#!/bin/zsh
# QD Up: the disc build carries none of NMS's debug status text (GFX.C, QDUP_DEBUG_TEXT; owner's decision 2026-11-18).
#   tests/gameplay/qdup-nodebug.sh BUILD_DIR
# The three status lines ("Player %d, Cue Angle", "Balls Moving", "Yd = ... Stat") are compiled only into tool builds; the foul
# and AIM MODE line stays (the PC shows it in its panel). PASS when no status format string is in qdup.exe and the AIM MODE one is.
B=$1
fails=0
for s in "Cue Angle = %d" "Balls Moving %3s" "Stat = %s"; do
  if LC_ALL=C grep -a -q -F "$s" $B/qdup.exe; then echo "FAIL  debug text \"$s\" is in the disc build"; fails=$((fails+1)); else echo "PASS  no \"$s\" in the disc build"; fi
done
if LC_ALL=C grep -a -q -F "AIM MODE" $B/qdup.exe; then echo "PASS  the AIM MODE / foul line stays"; else echo "FAIL  the AIM MODE / foul line is gone"; fails=$((fails+1)); fi
# The in-play hints (qdup/new/hints.c) fit the screen: 8-pixel cells from x 16, so 32 characters end by x 272, 48 pixels inside
# the 320-pixel field (docs/DECISIONS.md, the hints row).
root=$(cd "$(dirname "$0")/../.." && pwd)
long=$(/usr/bin/awk -F'"' '/FntPrint\("/{s=$2; sub(/\\n$/,"",s); if(length(s)>32) print length(s)": "s}' $root/qdup/new/hints.c)
if [ -z "$long" ]; then echo "PASS  every hint line is 32 characters or fewer"; else echo "FAIL  hint lines over 32 characters: $long"; fails=$((fails+1)); fi
exit $((fails>0))
