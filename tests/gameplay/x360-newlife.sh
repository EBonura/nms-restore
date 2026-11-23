#!/bin/zsh
# 360X: a new life starts at the start position. Mission 1, 25 thrust bursts with turns (the craft gets at least 30 pixels
# from its start in life 1), the life ends, the next life. BUILD_DIR is the test disc t-newlife (X360_CFLAGS='-DX360_TEST_HEADING=16
# -DX360_TEST_DIE_AT=1100'): every life starts on frame 16, to the right, as the script was laid out before lives started pointing
# up (2026-11-18), and the first life's shields go to 0 at game cycle 1100 so it ends as a hit ends it; it ended with Select,
# QUIT until QUIT went to the main menu (owner's decision, 2026-11-18).
# (Life 2 is found by the game's state, not by a tick number: the first position the craft takes once the ship count has fallen.)
# The craft must be at Mission 1's start (780, 874; CONTROL.C startx, starty), not where the last life ended.
#   tests/gameplay/x360-newlife.sh BUILD_DIR OUT_DIR
root=$(cd "$(dirname "$0")/../.." && pwd)
B=$1; O=$2; mkdir -p $O
boot=$(python3 -c "print(','.join(f'{t}:start' for t in range(150,2000,200)))")
fails=0
# The input script starts from the moment the mission starts (Init_Level gives the craft fuel), not from a
# tick number: anchor() runs the boot presses alone and finds that tick, and t N gives tick N of the original
# timing (the mission started at tick 2664) counted from there, so a faster or slower load cannot break it.
ANCHOR_REF=2664
anchor() { # build dir, out dir, extra emulator flags
  python3 -I $root/tools/x360_run.py cd $2/anchor --build $1 --press "$boot" --steps 1500000000 --watch fuel ${@:3} >/dev/null 2>&1
  A=$(/usr/bin/awk -F, '{sub(/\r$/,"")} NR>1 && $1>100 && $2>0 {print $1; exit}' $2/anchor/vars.csv)
  [ -n "$A" ] || { echo "FAIL  the mission never started in the boot run"; exit 1; }
}
t() { echo $(( A + $1 - ANCHOR_REF )); }
burst() { # seed first_tick bursts: thrust bursts of 20 to 70 ticks with turns and pauses
  python3 -I -c "
s=$1; t=$2; out=[]
def r():
    global s
    s=(s*1103515245+12345)&0x7fffffff
    return s>>8
for i in range($3):
    th=20+r()%50
    out.append(f'{t}:cross:{th}')
    k=r()%3
    if k==0: out.append(f'{t}:left:{10+r()%30}')
    elif k==1: out.append(f'{t}:right:{10+r()%30}')
    t+=th+10+r()%30
print(','.join(out))"
}
for setup in digital dpad; do
  extra=(); [ $setup = dpad ] && extra=(--analog)
  mkdir -p $O/$setup
  anchor $B $O/$setup $extra
  bootc=$(python3 -c "print(','.join(f'{t}:start' for t in range(150,2000,200) if t<$((A-100))))")
  more=$(python3 -c "print(','.join(f'{t}:start' for t in range($(t 5900),$(t 9000),300)))")
  P="$bootc,$(burst 7 $(t 3000) 25),$more"
  python3 -I $root/tools/x360_run.py cd $O/$setup --build $B --press "$P" --steps 3200000000 --shots 200 --watch gamecycle craftx crafty craftremaining pause $extra >/dev/null 2>&1
  y1=$(/usr/bin/awk -F, -v T5200=$(t 5200) -v T5300=$(t 5300) -v T9000=$(t 9000) '{sub(/\r$/,"")} $1==T5200{print $4}' $O/$setup/vars.csv)
  x1=$(/usr/bin/awk -F, -v T5200=$(t 5200) -v T5300=$(t 5300) -v T9000=$(t 9000) '{sub(/\r$/,"")} $1==T5200{print $3}' $O/$setup/vars.csv)
  # life 2: the craft's first position after the ship count falls to 4 that differs from the one the count fell with (the
  # mission reloads behind the briefing, where craftx and crafty still hold life 1's last values)
  l2=$(/usr/bin/awk -F, '{sub(/\r$/,"")} NR>1 && $5==4 && s=="" {s=$3","$4} NR>1 && $5==4 && s!="" && ($3","$4)!=s {print $3, $4; exit}' $O/$setup/vars.csv)
  x2=${l2% *}; y2=${l2#* }
  r=$(/usr/bin/awk -F, -v T5200=$(t 5200) -v T5300=$(t 5300) -v T9000=$(t 9000) '{sub(/\r$/,"")} $1==T9000{print $5}' $O/$setup/vars.csv)
  far=$(/usr/bin/awk -F, -v a=$(t 2900) -v b=$(t 5200) '{sub(/\r$/,"")} $1>=a && $1<=b && $3!=0 && $3<100000 {dx=$3-780; dy=$4-874; if (dx*dx+dy*dy>m) m=dx*dx+dy*dy} END{print (m>900)?1:0}' $O/$setup/vars.csv)
  paused=$(/usr/bin/awk -F, -v T=$(t 2900) '{sub(/\r$/,"")} $1==T{print $6}' $O/$setup/vars.csv)
  if [ "$paused" = 0 ] && [ -n "$y2" ] && [ $x2 -ge 777 ] && [ $x2 -le 783 ] && [ $y2 -ge 871 ] && [ $y2 -le 934 ] && [ $far = 1 ] && [ "$r" = 4 ]; then echo "PASS  $setup  new life starts at the start (life 1 ended at $x1, $y1; life 2 starts at $x2, $y2; ships left $r, not paused at the start)"; else echo "FAIL  $setup  new life starts at $x2, $y2 (life 1 ended at $x1, $y1; ships left $r)"; fails=$((fails+1)); fi
done
exit $((fails>0))
