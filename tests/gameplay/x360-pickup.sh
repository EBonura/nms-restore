#!/bin/zsh
# 360X pick-ups (PC OLD.C:233-287: fuel, shield, gravity reverse), on three TEST discs. No version creates a pick-up and no map has one,
# so a test build puts one at the craft at game cycle 100 in Mission 6 (the pod's art stands in), with the gauges at a baseline (fuel 96 of 192, shields 64 of 127).
#   tests/gameplay/x360-pickup.sh FUEL_DISC SHIELD_DISC GRAVITY_DISC OUT_DIR
# the discs are built with X360_CFLAGS='-DX360_TEST_PICKUP=0 -DX360_TEST_START_X=676 -DX360_TEST_START_Y=434' (fuel), =1 (shield), =2 (gravity); the start is
# Mission 6's old one (the game's is 820,434 since 2026-11-14: its ceiling is 15 pixels above, too low for the gravity check's 30 pixel rise). A pick-up has no control (the craft only has to be on it),
# so the pad setups do not apply: the boot presses are the only pad input.
# Prints PASS or FAIL per check; exit status 1 if any failed. Frames are in OUT_DIR.
# Every input script starts from the moment its mission starts, not from a tick number: anchor K runs the boot
# presses to Mission K alone and finds the first tick the craft has fuel (Init_Level sets it); `at K N` is tick N
# counted from there (the other scripts do the same with the mission's start at tick 2664).
root=$(cd "$(dirname "$0")/../.." && pwd)
B0=$1; B1=$2; B2=$3; O=$4; B=$B0
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
sx=(350 410 606 196 678 676 250 125 125); sy=(825 847 132 687 677 434 423 78 94)



anchor 6
A=${AK[6]}
# value of column $2 (name) of vars.csv $1 at game cycle $3 (first tick after the anchor that has it)
gc() { /usr/bin/awk -F, -v n=$2 -v g=$3 -v a=$A '{sub(/\r$/,"")} NR==1{for(i=1;i<=NF;i++){if($i==n)c=i; if($i=="gamecycle")h=i}} NR>1&&$1>a&&$h==g{print $c; exit}' $1/vars.csv; }
run() { # DISC NAME
  python3 -I $root/tools/x360_run.py cd $O/$2 --build $1 --press "$(boot 6 $A)" --steps 1700000000 --shots 90 --watch gamecycle fuel shields ships_gravity craftx crafty >/dev/null 2>&1
  echo "--- trace ($2): game cycle, fuel shields gravity craftx crafty"
  for g in 99 102 130 170 200 260 340; do echo "   $g: $(gc $O/$2 fuel $g) $(gc $O/$2 shields $g) $(gc $O/$2 ships_gravity $g) $(gc $O/$2 craftx $g) $(gc $O/$2 crafty $g)"; done
}
# fuel: 96 at the baseline, 48 more over 64 cycles (3 every 4), then it stays
run $B0 fuel
f0=$(gc $O/fuel fuel 102); f1=$(gc $O/fuel fuel 130); f2=$(gc $O/fuel fuel 200); f3=$(gc $O/fuel fuel 340); s2=$(gc $O/fuel shields 200); g2=$(gc $O/fuel ships_gravity 130)
check "Fuel: from the baseline 96 the pick-up fills it gradually (3 every 4 cycles: $f0 at cycle 102, $f1 at 130, $f2 at 200)" $([ "$f0" -ge 96 ] && [ "$f0" -le 99 ] && [ "$f1" -gt 99 ] && [ "$f1" -lt 144 ] && [ "$f2" = 144 ] && echo 1 || echo 0)
check "Fuel: taken once (fuel $f3 at cycle 340, no second fill), shields not raised ($s2, 64 baseline), gravity untouched ($g2)" $([ "$f3" = 144 ] && [ "$s2" -le 64 ] && [ "$g2" = 0 ] && echo 1 || echo 0)
# shield: 64, 32 more over 64 cycles (1 every 2). The craft at Mission 6's start loses a shield point about every 7 cycles to the floor (seen with no
# pick-up, shields 113 at cycle 99 falling to 34 at 340: not this work), so the check is the rise and the peak: +32 less about 8 lost, 85 to 90
run $B1 shield
h0=$(gc $O/shield shields 102); h1=$(gc $O/shield shields 130); h2=$(gc $O/shield shields 170); h3=$(gc $O/shield shields 340); fs=$(gc $O/shield fuel 200)
hp=$(/usr/bin/awk -F, -v a=$A '{sub(/\r$/,"")} NR==1{for(i=1;i<=NF;i++){if($i=="shields")c=i; if($i=="gamecycle")h=i}} NR>1&&$1>a&&$h>=102&&$h<=260&&$c>m{m=$c} END{print m+0}' $O/shield/vars.csv)
check "Shield: from the baseline 64 the pick-up fills it gradually ($h0 at cycle 102, $h1 at 130, $h2 at 170, peak $hp)" $([ "$h0" -ge 64 ] && [ "$h0" -le 66 ] && [ "$h1" -gt 70 ] && [ "$hp" -ge 82 ] && [ "$hp" -le 96 ] && echo 1 || echo 0)
check "Shield: taken once (peak $hp is no more than 96, shields $h3 at cycle 340), fuel untouched ($fs)" $([ "$hp" -le 96 ] && [ "$h3" -lt "$hp" ] && [ "$fs" = 96 ] && echo 1 || echo 0)
# gravity: 2048 cycles of reversed gravity, the craft rises
run $B2 gravity
r0=$(gc $O/gravity ships_gravity 102); r1=$(gc $O/gravity ships_gravity 200); y0=$(gc $O/gravity crafty 104); y1=$(gc $O/gravity crafty 340)
check "Gravity: reversed for 2048 cycles, counting down (gravity $r0 at cycle 102, $r1 at 200)" $([ "$r0" -gt 1900 ] && [ "$r0" -le 2048 ] && [ "$r1" -lt "$r0" ] && [ "$r1" -gt 1800 ] && echo 1 || echo 0)
check "Gravity: the craft rises (y $y0 at cycle 104, $y1 at 340)" $([ "$y1" -lt $((y0-30)) ] && echo 1 || echo 0)
echo "frames: $O/*/tick-*.ppm"
exit $((fails>0))
