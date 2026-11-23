#!/bin/zsh
# 360X teleporters (AI.DAT ai_tele1 and ai_tele2, set_level; Saturn AI_COMMAND_SET_LEVEL), on two TEST discs. No map places one, so a test build puts
# one at the craft at game cycle 100 of Mission 1 (once; a creature of the mission's art stands in).
#   tests/gameplay/x360-tele.sh TELE1_DISC TELE2_DISC OUT_DIR
# built with X360_CFLAGS=-DX360_TEST_TELE=1 and =2. ai_tele1 in Mission 1 (level 0) sends the craft to level 1 (Mission 2); ai_tele2 to level 5 (Mission 6).
# (Mission 6's old start drained shields; only the first 300 ticks of play are checked, as before.)
# A teleporter has no control. The new mission opens on its briefing, as any mission does, so Start presses (at 850, 1050 and 1250 ticks from the anchor)
# begin it; the boot presses and these are the only pad input.
# Prints PASS or FAIL per check; exit status 1 if any failed. Frames are in OUT_DIR.
# Every input script starts from the moment its mission starts, not from a tick number: anchor K runs the boot
# presses to Mission K alone and finds the first tick the craft has fuel (Init_Level sets it); `at K N` is tick N
# counted from there (the other scripts do the same with the mission's start at tick 2664).
root=$(cd "$(dirname "$0")/../.." && pwd)
B1=$1; B2=$2; O=$3; B=$B1
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
sx=(780 410 574 431 703 820 246 198 268); sy=(874 728 98 612 599 446 414 422 371)



anchor 1
A=${AK[1]}
sx=(780 410 574 431 703 820 246 198 268); sy=(874 728 98 612 599 446 414 422 371)
run() { # DISC NAME TARGET (0-based level): the level it reaches and where the craft stands
  python3 -I $root/tools/x360_run.py cd $O/$2 --build $1 --press "$(boot 1 $A),$(at 1 850):start,$(at 1 1050):start,$(at 1 1250):start" --steps 2300000000 --shots 200 --watch gamecycle level craftx crafty craftdestroyed >/dev/null 2>&1
  echo "--- trace ($2): tick, game cycle, level, craft x y, destroyed (every 100 ticks from the anchor)"
  /usr/bin/awk -F, -v a=$A 'NR>1&&$1>a&&(($1-a)%100==0){print "   "$1-a": "$2" "$3" "$4" "$5" "$6}' $O/$2/vars.csv | head -30
  l0=$(/usr/bin/awk -F, -v a=$A 'NR>1&&$1>a&&$2>=50&&$2<=90{print $3; exit}' $O/$2/vars.csv)
  tl=$(/usr/bin/awk -F, -v a=$A -v t=$3 'NR>1&&$1>a&&$3==t{print $1; exit}' $O/$2/vars.csv)
  [ -n "$tl" ] || tl=0
  lend=$(/usr/bin/awk -F, 'NR>1&&$1>100{l=$3} END{print l}' $O/$2/vars.csv)
  # the new mission's first playing tick: after the teleport, the game cycle back at 40 or more (it counts on the briefing screens too, and is 2 while the mission loads)
  t0=$(/usr/bin/awk -F, -v t=$tl 'NR>1&&$1>t+300&&$2>=40&&$2<60{print $1; exit}' $O/$2/vars.csv)
  [ -n "$t0" ] || t0=0
  cx=$(val $O/$2 craftx $t0); cy=$(val $O/$2 crafty $t0)
  dm=$(/usr/bin/awk -F, -v t=$t0 'NR>1&&$1>=t&&$1<=t+300&&$6>m{m=$6} END{print m+0}' $O/$2/vars.csv)
  check "[$2] Mission 1 is level $l0 before the teleporter" $([ "$l0" = 0 ] && echo 1 || echo 0)
  check "[$2] The craft reaches level $3 (first at tick $tl, level $lend at the end; the new mission plays from tick $t0)" $([ "$tl" -gt 0 ] && [ "$lend" = "$3" ] && echo 1 || echo 0)
  tx=${sx[$(($3+1))]}; ty=${sy[$(($3+1))]}
  dx=$((cx>tx?cx-tx:tx-cx)); dy=$((cy>ty?cy-ty:ty-cy))
  check "[$2] It stands at that mission's start ($cx,$cy against $tx,$ty) and is not destroyed (max $dm)" $([ "$dx" -le 20 ] && [ "$dy" -le 20 ] && [ "$dm" = 0 ] && echo 1 || echo 0)
}
run $B1 tele1 1
run $B2 tele2 5
echo "frames: $O/*/tick-*.ppm"
exit $((fails>0))
