#!/bin/zsh
# 360X gameplay check, Mission 1, through the pad the emulator presents (the layer's own poll).
#   tests/gameplay/x360.sh BUILD_DIR OUT_DIR
# Three setups: digital pad; DualShock in analog mode with the direction pad; the same with the left
# stick for left and right. One assertion per control, on game variables, with a frame each (OUT_DIR).
# Prints PASS or FAIL per control and setup; exit status 1 if any failed.
root=$(cd "$(dirname "$0")/../.." && pwd)
B=$1; O=$2
mkdir -p $O
boot=$(python3 -c "print(','.join(f'{t}:start' for t in range(150,2000,200)))")
fails=0
# The mission starts when its load ends, which is when the layer or the data change; the input script
# starts from that moment, not from a tick number: anchor() runs the boot presses alone and finds the first
# tick the craft has fuel (Init_Level sets it), and t N gives tick N of the script's original timing
# (the mission started at tick 2664) counted from there.
ANCHOR_REF=2664
anchor() { # build dir, out dir, extra emulator flags
  python3 -I $root/tools/x360_run.py cd $2/anchor --build $1 --press "$boot" --steps 1500000000 --watch fuel ${@:3} >/dev/null 2>&1
  A=$(/usr/bin/awk -F, '{sub(/\r$/,"")} NR>1 && $1>100 && $2>0 {print $1; exit}' $2/anchor/vars.csv)
  [ -n "$A" ] || { echo "FAIL  the mission never started in the boot run"; exit 1; }
}
t() { echo $(( A + $1 - ANCHOR_REF )); }
# value of column $2 (name) of vars.csv $1 at tick $3
val() { /usr/bin/awk -F, -v n=$2 -v t=$3 '{sub(/\r$/,"")} NR==1{for(i=1;i<=NF;i++)if($i==n)c=i} NR>1&&$1==t{print $c; exit}' $1/vars.csv; }
maxv() { /usr/bin/awk -F, -v n=$2 -v a=$3 -v b=$4 '{sub(/\r$/,"")} NR==1{for(i=1;i<=NF;i++)if($i==n)c=i} NR>1&&$1>=a&&$1<=b&&$c>m{m=$c} END{print m+0}' $1/vars.csv; }
sg() { echo $(( ((($1) % 64 + 64 + 32) % 64) - 32 )); }
check() { # setup label ok
  if [ "$3" = 1 ]; then echo "PASS  $1  $2"; else echo "FAIL  $1  $2"; fails=$((fails+1)); fi
}
for setup in digital dpad stick; do
  case $setup in
    digital) extra=();         L="left";  R="right";;
    dpad)    extra=(--analog); L="left";  R="right";;
    stick)   extra=(--analog); L="lstick=0/128"; R="lstick=255/128";;
  esac
  D=$O/$setup
  mkdir -p $D
  anchor $B $D $extra
  # the boot presses stop before the mission starts: a Start after that would pause the game
  bootc=$(python3 -c "print(','.join(f'{t}:start' for t in range(150,2000,200) if t<$((A-100))))")
  P="$bootc"
  P="$P,$(t 2960):$L:40,$(t 3060):$R:40,$(t 3160):cross:60,$(t 3260):square:30,$(t 3360):r1:6,$(t 3460):l1:6"
  P="$P,$(t 3560):select:6,$(t 3660):start:6,$(t 3760):start:6,$(t 3860):start:6"
  D=$O/$setup
  python3 -I $root/tools/x360_run.py cd $D --build $B --press "$P" --steps 2400000000 --shots 100 --watch gamecycle angle fuel crafty boost fire_delay weapontype whichmenu pause $extra >/dev/null 2>&1
  check $setup "The game is running when the checks start (pause $(val $D pause $(t 2900)) at the mission start)" $([ "$(val $D pause $(t 2900))" = 0 ] && echo 1 || echo 0)
  a0=$(val $D angle $(t 2955)); a1=$(val $D angle $(t 3005)); a2=$(val $D angle $(t 3055)); a3=$(val $D angle $(t 3105))
  dl=$(sg $((a1-a0))); dr=$(sg $((a3-a2)))
  check $setup "Left turns the craft one way (angle $a0 -> $a1)" $([ $dl -lt 0 ] && echo 1 || echo 0)
  check $setup "Right turns it the other way (angle $a2 -> $a3)" $([ $dr -gt 0 ] && echo 1 || echo 0)
  f0=$(val $D fuel $(t 3155)); f1=$(val $D fuel $(t 3225)); y0=$(val $D crafty $(t 3155)); y1=$(val $D crafty $(t 3225))
  y2=$(val $D crafty $(t 3500)); y3=$(val $D crafty $(t 3550))
  check $setup "Cross burns fuel (fuel $f0 -> $f1)" $([ $f1 -lt $f0 ] && echo 1 || echo 0)
  bt=$(maxv $D boost $(t 3160) $(t 3220)); bn=$(maxv $D boost $(t 3500) $(t 3550))
  check $setup "Cross applies thrust (boost $bt while held, $bn after; the craft starts on the map floor, so its y says nothing)" $([ "$bt" = 4 ] && [ "$bn" = 0 ] && echo 1 || echo 0)
  check $setup "Fire starts a shot (fire_delay max $(maxv $D fire_delay $(t 3260) $(t 3340)), 0 before)" $([ $(maxv $D fire_delay $(t 3260) $(t 3340)) -gt 0 ] && [ $(val $D fire_delay $(t 3255)) = 0 ] && echo 1 || echo 0)
  w0=$(val $D weapontype $(t 3355)); w1=$(val $D weapontype $(t 3440)); w2=$(val $D weapontype $(t 3540))
  check $setup "R1 then L1 change the weapon (weapontype $w0 -> $w1 -> $w2)" $([ $w1 -eq $((w0+1)) ] && [ $w2 -eq $w0 ] && echo 1 || echo 0)
  check $setup "Select opens the pause menu, Start on RESUME returns (whichmenu $(val $D whichmenu $(t 3555)) -> $(val $D whichmenu $(t 3620)) -> $(val $D whichmenu $(t 3700)))" $([ $(val $D whichmenu $(t 3620)) = 1 ] && [ $(val $D whichmenu $(t 3700)) = 0 ] && echo 1 || echo 0)
  check $setup "Start pauses and unpauses (pause $(val $D pause $(t 3755)) -> $(val $D pause $(t 3820)) -> $(val $D pause $(t 3920)))" $([ $(val $D pause $(t 3820)) != 0 ] && [ $(val $D pause $(t 3920)) = 0 ] && echo 1 || echo 0)
done
echo "frames: $O/<setup>/tick-*.ppm (2900 before, 3000 turned left, 3100 turned back, 3200 thrust, 3500 free fall)"
exit $((fails>0))
