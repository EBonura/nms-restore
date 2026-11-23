#!/bin/zsh
# 360X pod (PC ship_pickup_pod, ai_pod, the rope), on a TEST disc, through the pad, in three setups (digital pad; DualShock analog with the direction pad; the same with the left stick): pick up, carry, drop.
#   tests/gameplay/x360-pod.sh BUILD_DIR OUT_DIR [PLACED_DIR]
# BUILD_DIR is a disc built with X360_CFLAGS='-DX360_TEST_POD_X=650 -DX360_TEST_POD_Y=430': one pod (BMF 171, podtemp) is
#   placed in Mission 6 (the only mission that loads its art), 26 pixels left of the craft's start, which the disc sets to its old place, 676,434
#   (-DX360_TEST_START_X=676 -DX360_TEST_START_Y=434; the game's start is 820,434 since 2026-11-14). No map places one.
# PLACED_DIR (optional) is a disc built with X360_CFLAGS='-DX360_TEST_POD_WATCH -DX360_TEST_START_X=615 -DX360_TEST_START_Y=441': the game's own pod
#   (Add_Level_Pod, 585,441) with the craft started 30 pixels to its right; the same checks run against it.
# Prints PASS or FAIL per check; exit status 1 if any failed. Frames are in OUT_DIR.
# Every input script starts from the moment its mission starts, not from a tick number: anchor K runs the boot
# presses to Mission K alone and finds the first tick the craft has fuel (Init_Level sets it); `at K N` is tick N
# counted from there (the other scripts do the same with the mission's start at tick 2664).
root=$(cd "$(dirname "$0")/../.." && pwd)
B=$1; O=$2; B2=$3
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



# --- Mission 6: the pod stands, a Triangle press takes it, it follows the craft on the rope, a second press drops it.
# The craft turns left a quarter-ish (the start sits under the floor's lip: straight up is blocked), then thrusts; the pod is lifted
# and carried; the second press drops it high above the floor, so a fall is visible. Thresholds, from the traces printed below:
#   the pod is 26 px from the craft at the press and the rope holds it within about 33 px while carried (seen 33 to 34): limit 45;
#   it moves more than 100 px from its place (seen 234 to 235 in the three setups): limit 100;
#   a dropped pod falls (seen 24 px of fall in the 60 ticks after a drop at y 327): any fall counts.
anchor 6
A=${AK[6]}
podrun() { # DISC PODX PODY TAG: the three pad setups against one disc
  local BD=$1 PX=$2 PY=$3 TAG=$4 setup
  for setup in digital dpad stick; do
  extra=(); L="left"
  [ $setup = dpad ] && extra=(--analog)
  [ $setup = stick ] && { extra=(--analog); L="lstick=0/128"; }
  D=$O/$TAG-$setup
  P="$(boot 6 $A),$(at 6 120):triangle:4,$(at 6 160):$L:30,$(at 6 220):cross:450,$(at 6 640):triangle:4"
  python3 -I $root/tools/x360_run.py cd $D --build $BD --press "$P" --steps 2600000000 --shots 50 ${extra[@]} --watch gamecycle craftx crafty testpodx testpody testpodatt testballx testbally >/dev/null 2>&1
  echo "--- trace ($setup): tick craftx crafty podx pody attached ballx bally, every 40 ticks"
  /usr/bin/awk -F, 'NR>1&&$1>=a&&$1<=a+680&&(($1-a)%40==0){print "   "$1-a": "$3" "$4" "$5" "$6" "$7" "$8" "$9}' a=$A $D/vars.csv
  px0=$(val $D testpodx $((A+100))); py0=$(val $D testpody $((A+100)))
  att0=$(val $D testpodatt $((A+110)))
  check "[$setup] The pod stands where it was placed until it is taken ($PX,$PY; read ($px0,$py0))" $([ "$px0" = $PX ] && [ "$py0" = $PY ] && echo 1 || echo 0)
  check "[$setup] Nothing is carried before the press (pod_attached $att0)" $([ "$att0" = 0 ] && echo 1 || echo 0)
  t1=$(first $D testpodatt 1 $A)
  check "[$setup] A Triangle press with the pod in range takes it (pod_attached 1 at tick ${t1:-never}, press at $((A+120)))" $([ -n "$t1" ] && [ "$t1" -ge $((A+120)) ] && [ "$t1" -le $((A+140)) ] && echo 1 || echo 0)
  read dmax moved lost <<< $(/usr/bin/awk -F, -v a=$((A+200)) -v b=$((A+635)) -v px=$PX -v py=$PY '{sub(/\r$/,"")} NR>1&&$1>=a&&$1<=b{dx=$5-$3;dy=$6-$4;d=sqrt(dx*dx+dy*dy);if(d>m)m=d; mx=($5>px?$5-px:px-$5); my=($6>py?$6-py:py-$6); if(mx+my>moved)moved=mx+my; if($7!=1)lost++} END{printf "%d %d %d\n",m,moved,lost+0}' $D/vars.csv)
  check "[$setup] The carried pod follows the craft: it moved $moved px from its place, never more than $dmax px from the craft" $([ "$moved" -gt 100 ] && [ "$dmax" -le 45 ] && echo 1 || echo 0)
  check "[$setup] It stayed taken the whole carry ($lost ticks not attached)" $([ "$lost" = 0 ] && echo 1 || echo 0)
  t2=$(first $D testpodatt 0 $((A+645)))
  check "[$setup] The second press drops it (pod_attached 0 at tick ${t2:-never}, press at $((A+640)))" $([ -n "$t2" ] && [ "$t2" -le $((A+665)) ] && echo 1 || echo 0)
  yd=$(val $D testpody $t2); y2=$(val $D testpody $((t2+60)))
  check "[$setup] A dropped pod falls (y $yd, then $y2 60 ticks later)" $([ -n "$y2" ] && [ "$y2" -gt "$yd" ] && echo 1 || echo 0)
  done
}
podrun $B 650 430 pod
# the pod the game places in Mission 6 (candidate A, 585,441), through the same presses, from a start 30 pixels to its right (the test disc's start, 615,441)
[ -n "$B2" ] && podrun $B2 585 441 podA
echo "frames: $O/pod-*/tick-*.ppm"
exit $((fails>0))
