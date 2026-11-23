#!/bin/zsh
# 360X laser creature's beam (PC add_line), on a TEST disc, through the pad.
#   tests/gameplay/x360-laser.sh BUILD_DIR OUT_DIR
# BUILD_DIR is a disc built with X360_CFLAGS='-DX360_TEST_WARP=100 -DX360_TEST_WARP_EVERY=900 -DX360_TEST_WARP_HOLD=700
#   -DX360_TEST_WARP_NAMES=109 -DX360_TEST_WARP_DX=26 -DX360_TEST_WARP_DY=0': from game cycle 100 the craft is put 26 pixels
#   right of Mission 1's laser creature (lazerl1, BMF number 109), inside its range, for 700 game cycles.
# Prints PASS or FAIL per check; exit status 1 if any failed. Frames are in OUT_DIR.
# Every input script starts from the moment its mission starts, not from a tick number: anchor K runs the boot
# presses to Mission K alone and finds the first tick the craft has fuel (Init_Level sets it); `at K N` is tick N
# counted from there (the other scripts do the same with the mission's start at tick 2664).
root=$(cd "$(dirname "$0")/../.." && pwd)
B=$1; O=$2
mkdir -p $O
fails=0
data=${X360_DATA:-$root/build/originals/360x}
# boot presses that reach mission K (1 to 9) and then start it
boot() { # K [LIMIT]: with LIMIT, no Start press at or after tick LIMIT-100 (a Start after the mission began would pause it)
  python3 -I -c "
k=$1; lim=int('${2:-999999}')-100
p=['100:start','150:start','350:start','550:start','750:start']
for i in range(k-1): p.append(f'{1000+40*i}:right')
t=1000+40*max(k-1,0)+100
if t<lim: p.append(f'{t}:start')
for j in range(1,5):
    if t+200*j<lim: p.append(f'{t+200*j}:start')
print(','.join(p))"
}
typeset -A AK
anchor() { # K: the tick Mission K starts at, from a run of the boot presses alone
  [ -n "${AK[$1]}" ] && return
  python3 -I $root/tools/x360_run.py cd $O/anchor$1 --build $B --press "$(boot $1)" --steps 1100000000 --watch fuel >/dev/null 2>&1
  AK[$1]=$(/usr/bin/awk -F, '{sub(/\r$/,"")} NR>1 && $1>100 && $2>0 {print $1; exit}' $O/anchor$1/vars.csv)
  [ -n "${AK[$1]}" ] || { echo "FAIL  Mission $1 never started in the boot run"; exit 1; }
}
at() { echo $(( ${AK[$1]} + $2 )); }
# value of column $2 (name) of vars.csv $1 at tick $3
val() { /usr/bin/awk -F, -v n=$2 -v t=$3 '{sub(/\r$/,"")} NR==1{for(i=1;i<=NF;i++)if($i==n)c=i} NR>1&&$1==t{print $c; exit}' $1/vars.csv; }
# first tick (from $4) at which column $2 of vars.csv $1 equals $3
first() { /usr/bin/awk -F, -v n=$2 -v v=$3 -v a=$4 '{sub(/\r$/,"")} NR==1{for(i=1;i<=NF;i++)if($i==n)c=i} NR>1&&$1>=a&&$c==v{print $1; exit}' $1/vars.csv; }
check() { if [ "$2" = 1 ]; then echo "PASS  $1"; else echo "FAIL  $1"; fails=$((fails+1)); fi }
sx=(350 410 606 196 678 820 250 125 125); sy=(825 847 132 687 677 434 423 78 94)


# --- Mission 1: the beam is five lines a cycle while the laser fires, and none before the craft is in range
anchor 1
D=$O/beam
python3 -I $root/tools/x360_run.py cd $D --build $B --press "$(boot 1 ${AK[1]})" --steps 1300000000 --shots 25 --watch gamecycle glowcount craftx crafty >/dev/null 2>&1
before=$(/usr/bin/awk -F, -v a=${AK[1]} '{sub(/\r$/,"")} NR>1&&$1>=a&&$1<a+150&&$3>m{m=$3} END{print m+0}' $D/vars.csv)
mx=$(/usr/bin/awk -F, -v a=${AK[1]} '{sub(/\r$/,"")} NR>1&&$1>=a&&$3>m{m=$3} END{print m+0}' $D/vars.csv)
t5=$(first $D glowcount 5 ${AK[1]})
n5=$(/usr/bin/awk -F, -v a=${t5:-0} '{sub(/\r$/,"")} NR>1&&$1>=a&&$1<a+300&&$3==5{n++} END{print n+0}' $D/vars.csv)
check "No beam before the craft is in range (most lines in the first 150 ticks of play: $before)" $([ "$before" = 0 ] && echo 1 || echo 0)
check "The laser fires five lines a cycle (most lines in a cycle: $mx, first at tick ${t5:-never})" $([ "$mx" = 5 ] && [ -n "$t5" ] && echo 1 || echo 0)
check "The beam stays on while the craft is near ($n5 of the 300 ticks after it began show five lines)" $([ "$n5" -gt 100 ] && echo 1 || echo 0)
echo "frames: $D/tick-*.ppm"
exit $((fails>0))
