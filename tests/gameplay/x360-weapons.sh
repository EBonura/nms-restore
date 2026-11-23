#!/bin/zsh
# 360X weapons, on a TEST disc, through the pad in three setups (digital pad; DualShock analog with the direction pad; the same with the left stick).
#   tests/gameplay/x360-weapons.sh BUILD_DIR POCKET_DIR RING_DIR SHIELD_DIR OPEN_DIR POCKET2_DIR OUT_DIR
# BUILD_DIR is a disc built with X360_CFLAGS='-DX360_TEST_FIRE -DX360_TEST_WARP=100 -DX360_TEST_WARP_EVERY=900 -DX360_TEST_WARP_HOLD=700
#   -DX360_TEST_WARP_NAMES=109 -DX360_TEST_WARP_DX=30 -DX360_TEST_WARP_DY=150': from game cycle 100 the craft is put at (434,804) of Mission 1, still, its heading
#   east with free space ahead; at cycle 110 a plunger is put at (520,770), 34 pixels above the line of a straight shot. A weapon is chosen with R1 (one step up
#   the list: missile 0, laser 1, buckshot 2, proton 3, chain 4, prong 5, homing 6, circle 7, shield 8) and fired with Square.
# POCKET_DIR is a disc built with X360_CFLAGS='-DX360_TEST_FIRE -DX360_TEST_AT=100 -DX360_TEST_AT_X=40 -DX360_TEST_AT_Y=178': from game cycle 100 the craft is held
#   still at (40,178) of Mission 1, inside a rock pocket about 60 pixels across (walls 29 to 68 pixels away in every one of eight directions).
# RING_DIR is the first disc built again with -DX360_TEST_FIRE_DY=0: the plunger stands in the line of fire, 86 pixels east of the craft.
# SHIELD_DIR is the first disc built again with -DX360_TEST_FIRE_DX=0 -DX360_TEST_FIRE_DY=16: the plunger stands 16 pixels below the craft, on the shield pieces' orbit.
# OPEN_DIR is a disc built with X360_CFLAGS='-DX360_TEST_FIRE -DX360_TEST_AT=100 -DX360_TEST_AT_X=120 -DX360_TEST_AT_Y=82 -DX360_TEST_NOHIT': the craft is held at (120,82) of
#   Mission 1, heading east, with 790 pixels of open ground ahead; NOHIT makes no shot hit a creature and no creature a homing target, so a shot ends by its time or a wall.
# POCKET2_DIR is the pocket disc built again with -DX360_TEST_NOHIT.
# Prints PASS or FAIL per check; exit status 1 if any failed. Frames are in OUT_DIR.
# Every input script starts from the moment its mission starts, not from a tick number: anchor K runs the boot
# presses to Mission K alone and finds the first tick the craft has fuel (Init_Level sets it); `at K N` is tick N
# counted from there (the other scripts do the same with the mission's start at tick 2664).
root=$(cd "$(dirname "$0")/../.." && pwd)
B=$1; B2=$2; B3=$3; B4=$4; B5=$5; B6=$6; O=$7
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



anchor 1
A=${AK[1]}
# fire weapon $1 at tick $2 after the anchor, in pad setup $3 (OUT subdir name $4); the craft is warped at cycle 100 (about tick 500 after the anchor)
fire() { # WEAPON NAME [EXTRA]
  local w=$1 n=$2; shift 2
  local P="$(boot 1 $A)" i
  if [ $w -gt 0 ]; then for i in $(seq 1 $w); do P="$P,$(at 1 $((100+i*50))):r1:10"; done; fi
  P="$P,$(at 1 600):square:10"
  python3 -I $root/tools/x360_run.py cd $O/$n --build $B --press "$P" --steps 1500000000 --shots 50 "$@" --watch gamecycle shotsontarget shotsfired weapontype testfire testfire+4 testfire+8 testfire+12 testfire+16 >/dev/null 2>&1
}
# the highest value of column $2 (name) of $1 after game cycle 150
mx() { /usr/bin/awk -F, -v n=$2 '{sub(/\r$/,"")} NR==1{for(i=1;i<=NF;i++){if($i==n)c=i; if($i=="gamecycle")g=i}} NR>1&&$1>100&&$g>=150&&$c>m{m=$c} END{print m+0}' $1/vars.csv; }
for setup in digital dpad stick; do
  extra=(); [ $setup != digital ] && extra=(--analog)
  # --- homing (weapon 6): the shot looks for the nearest creature, turns toward it and hits it; the missile (weapon 0) fired the same way passes under it
  fire 6 hom-$setup ${extra[@]}
  fire 0 ctl-$setup ${extra[@]}
  wt=$(mx $O/hom-$setup weapontype); hit=$(mx $O/hom-$setup shotsontarget); fired=$(mx $O/hom-$setup shotsfired); tg=$(mx $O/hom-$setup testfire+8)
  chit=$(mx $O/ctl-$setup shotsontarget); cf=$(mx $O/ctl-$setup shotsfired)
  echo "--- homing trace ($setup): game cycle, shots hit, shot x y, target sprite, target x y"
  /usr/bin/awk -F, '{sub(/\r$/,"")} NR>1&&$1>2800&&($1%40==0){print "   "$2": "$3" "$6" "$7" "$8" "$9" "$10}' $O/hom-$setup/vars.csv | awk '!s[$0]++' | head -8
  check "[$setup] R1 six times selects the homing weapon (weapon $wt) and Square fires it ($fired shot)" $([ "$wt" = 6 ] && [ "$fired" = 1 ] && echo 1 || echo 0)
  check "[$setup] The homing shot found a target (sprite $tg) and hit it ($hit hit)" $([ "$tg" -gt 0 ] && [ "$tg" -lt 256 ] && [ "$hit" = 1 ] && echo 1 || echo 0)
  check "[$setup] The same shot from the missile passes under it ($chit hit of $cf shot)" $([ "$cf" = 1 ] && [ "$chit" = 0 ] && echo 1 || echo 0)
done

# --- proton ball (weapon 3), in the pocket: it bounces off the walls on all four sides (the PC's four probes, 4 pixels left, right, above and below the ball)
for setup in digital dpad stick; do
  extra=(); L="left"
  [ $setup != digital ] && extra=(--analog)
  [ $setup = stick ] && L="lstick=0/128"
  D=$O/pro-$setup
  P="$(boot 1 $A),$(at 1 150):r1:10,$(at 1 200):r1:10,$(at 1 250):r1:10,$(at 1 400):$L:33,$(at 1 600):square:10"
  python3 -I $root/tools/x360_run.py cd $D --build $B2 --press "$P" --steps 1900000000 --shots 100 ${extra[@]} --watch gamecycle weapontype angle testball testball+4 testball+8 testball+12 testball+16 >/dev/null 2>&1
  # the ball's samples (x y dx dy), dx and dy as signed numbers; count the turns of each sign
  /usr/bin/awk -F, '{sub(/\r$/,"")} NR>1&&$1>2500&&$9!=0&&$5!=0{x=$5;y=$6;dx=$7;dy=$8; if(dx>2147483648)dx-=4294967296; if(dy>2147483648)dy-=4294967296; sx=(dx>0)-(dx<0); sy=(dy>0)-(dy<0); if(px!=""&&sx!=px&&sx!=0){ if(sx<0)xpm++; else xmp++ } if(py!=""&&sy!=py&&sy!=0){ if(sy<0)ypm++; else ymp++ } if(sx!=0)px=sx; if(sy!=0)py=sy; if(x<mnx||mnx=="")mnx=x; if(x>mxx)mxx=x; if(y<mny||mny=="")mny=y; if(y>mxy)mxy=y; n++} END{printf "%d %d %d %d %d %d %d %d %d\n",xpm,xmp,ypm,ymp,mnx,mxx,mny,mxy,n}' $D/vars.csv > $D/bounce.txt
  read xpm xmp ypm ymp mnx mxx mny mxy nn < $D/bounce.txt
  wt=$(mx $D weapontype)
  echo "--- proton ($setup): samples $nn, ball x $mnx to $mxx, y $mny to $mxy; turns of dx (+ to -, - to +): $xpm $xmp, of dy: $ypm $ymp"
  check "[$setup] R1 three times selects the proton weapon (weapon $wt) and the ball is fired ($nn samples)" $([ "$wt" = 3 ] && [ "$nn" -gt 20 ] && echo 1 || echo 0)
  check "[$setup] The ball bounces off the walls to its right and left (dx turned $xpm and $xmp times)" $([ "$xpm" -ge 1 ] && [ "$xmp" -ge 1 ] && echo 1 || echo 0)
  check "[$setup] The ball bounces off the walls below and above (dy turned $ypm and $ymp times)" $([ "$ypm" -ge 1 ] && [ "$ymp" -ge 1 ] && echo 1 || echo 0)
  check "[$setup] The ball is stopped by its shape, not its centre point: its y range is $((mxy-mny)) pixels (59 with the point test, 50 with the shape; x $((mxx-mnx)): 28 and 28)" $([ "$((mxy-mny))" -le 54 ] && echo 1 || echo 0)
  check "[$setup] The ball stays in the pocket (x $mnx to $mxx, y $mny to $mxy; the craft is at 40,178)" $([ "$mnx" -gt 0 ] && [ "$mxx" -lt 110 ] && [ "$mny" -gt 130 ] && [ "$mxy" -lt 230 ] && echo 1 || echo 0)
done

# --- ring weapon (weapon 7): three rings leave the craft and each draws a growing disc of light lines; their centres hit what is in their way
for setup in digital dpad stick; do
  extra=(); [ $setup != digital ] && extra=(--analog)
  D=$O/ring-$setup
  P="$(boot 1 $A)"; for i in 1 2 3 4 5 6 7; do P="$P,$(at 1 $((100+i*50))):r1:10"; done; P="$P,$(at 1 600):square:10"
  python3 -I $root/tools/x360_run.py cd $D --build $B3 --press "$P" --steps 1500000000 --shots 20 ${extra[@]} --watch gamecycle shotsontarget shotsfired weapontype glowcount >/dev/null 2>&1
  wt=$(mx $D weapontype); fired=$(mx $D shotsfired); hit=$(mx $D shotsontarget)
  gmax=$(/usr/bin/awk -F, -v lo=$((A+205)) '{sub(/\r$/,"")} NR==1{for(i=1;i<=NF;i++)if($i=="glowcount")c=i} NR>1&&$1>lo&&$c<1000&&$c>m{m=$c} END{print m+0}' $D/vars.csv)
  gbefore=$(/usr/bin/awk -F, -v lo=$((A+205)) -v hi=$((A+555)) '{sub(/\r$/,"")} NR==1{for(i=1;i<=NF;i++)if($i=="glowcount")c=i} NR>1&&$1>lo&&$1<hi&&$c<1000&&$c>m{m=$c} END{print m+0}' $D/vars.csv)
  echo "--- ring ($setup): fired $fired, hit $hit, most light lines in a cycle $gmax (before the shot $gbefore)"
  check "[$setup] R1 seven times selects the ring weapon (weapon $wt); Square fires three rings ($fired shots)" $([ "$wt" = 7 ] && [ "$fired" = 3 ] && echo 1 || echo 0)
  check "[$setup] The rings are drawn: up to $gmax light lines in a cycle, none before the shot ($gbefore)" $([ "$gmax" -ge 15 ] && [ "$gmax" -le 320 ] && [ "$gbefore" = 0 ] && echo 1 || echo 0)
  check "[$setup] The rings' centres hit the plunger in their way ($hit hits)" $([ "$hit" -ge 1 ] && echo 1 || echo 0)
done

# --- shield weapon (weapon 8): fires with no ammunition, three pieces orbit the craft at the PS1's radius 16, and a piece is spent when it hits a creature
for setup in digital dpad stick; do
  extra=(); [ $setup != digital ] && extra=(--analog)
  D=$O/shd-$setup
  P="$(boot 1 $A)"; for i in 1 2 3 4 5 6 7 8; do P="$P,$(at 1 $((100+i*50))):r1:10"; done; P="$P,$(at 1 700):square:10"
  python3 -I $root/tools/x360_run.py cd $D --build $B4 --press "$P" --steps 1700000000 --shots 100 ${extra[@]} --watch gamecycle shotsontarget shotsfired weapontype testshield testshield+4 testshield+8 testshield+12 testshield+16 >/dev/null 2>&1
  wt=$(mx $D weapontype); fired=$(mx $D shotsfired); hit=$(mx $D shotsontarget)
  # the first piece's distance from the craft while pieces were out (shots fired 3): count the samples and the range
  read ns dmin dmax <<< $(/usr/bin/awk -F, '{sub(/\r$/,"")} NR>1&&$1>2800&&$4==3&&$6!=0&&$8!=0&&$10!=0{dx=$6-$8;dy=$7-$9;d=sqrt(dx*dx+dy*dy); if(d<mn||mn=="")mn=d; if(d>mx)mx=d; n++} END{printf "%d %d %d\n",n,mn,mx}' $D/vars.csv)
  echo "--- shield ($setup): fired $fired, hit $hit, $ns samples of a piece at $dmin to $dmax pixels from the craft"
  check "[$setup] R1 eight times selects the shield weapon (weapon $wt) and Square fires it with no ammunition (3 pieces: $fired)" $([ "$wt" = 8 ] && [ "$fired" = 3 ] && echo 1 || echo 0)
  check "[$setup] A piece orbits the craft at radius 16 ($ns samples, $dmin to $dmax pixels)" $([ "$ns" -ge 20 ] && [ "$dmin" -ge 14 ] && [ "$dmax" -le 17 ] && echo 1 || echo 0)
  check "[$setup] The pieces hit what they pass over ($hit hits)" $([ "$hit" -ge 1 ] && echo 1 || echo 0)
done
# --- chain weapon (weapon 4): the shot is the chain link (NAME_CHAINB, 16 frames, one step every 5 game cycles, 4 pixels a cycle), ends at a wall by its shape
for setup in digital dpad stick; do
  extra=(); [ $setup != digital ] && extra=(--analog)
  D=$O/chn-$setup
  P="$(boot 1 $A)"; for i in 1 2 3 4; do P="$P,$(at 1 $((100+i*50))):r1:10"; done; P="$P,$(at 1 600):square:10"
  python3 -I $root/tools/x360_run.py cd $D --build $B --press "$P" --steps 1500000000 --shots 40 ${extra[@]} --watch gamecycle shotsfired weapontype testchain testchain+4 testchain+8 testchain+12 testchain+16 testchain+20 >/dev/null 2>&1
  wt=$(mx $D weapontype); fired=$(mx $D shotsfired)
  # samples while the shot flies (name flag 1): count, distinct frames, the frame sequence's steps, the speed
  read ns nfr mono dxs ex ey <<< $(/usr/bin/awk -F, '{sub(/\r$/,"")} NR==1{for(i=1;i<=NF;i++){if($i=="gamecycle")g=i; if($i=="testchain")c=i}} NR>1&&$g>150&&$(c+3)==1&&$c!=0{ if(g0!=$g){ g0=$g; if($c!=px){ if(px!="")dxs=$c-px; if(f!=$(c+2)){ if(f!=""&&$(c+2)!=(f+1)%16)bad=1; f=$(c+2); nf++ } n++; px=$c } } ex=$(c+4); ey=$(c+5)} END{printf "%d %d %d %d %d %d\n",n,nf,(bad==1?0:1),dxs,ex,ey}' $D/vars.csv)
  echo "--- chain ($setup): fired $fired, $ns cycles in flight, $nfr frames shown, frames in order $mono, x step $dxs, ended at $ex,$ey"
  check "[$setup] R1 four times selects the chain weapon (weapon $wt) and Square fires one shot ($fired)" $([ "$wt" = 4 ] && [ "$fired" = 1 ] && echo 1 || echo 0)
  check "[$setup] The shot is the chain link and flies ($ns cycles with its name)" $([ "$ns" -ge 10 ] && echo 1 || echo 0)
  check "[$setup] Its frames step in order, 0 up ($nfr frames shown), and it moves 4 pixels a cycle (step $dxs)" $([ "$nfr" -ge 3 ] && [ "$mono" = 1 ] && [ "$dxs" = 4 ] && echo 1 || echo 0)
  check "[$setup] It ends at a wall (ended at $ex,$ey)" $([ "$ex" != 0 ] && echo 1 || echo 0)
done
# --- impact pictures: the PC adds a small picture where the proton ball ends, on the prong, on the homing shot (wall and end of life) and on the ring; the laser and the missile
# add none of them (the missile has the Saturn's explosions). testimp counts the pictures added, by event: proton end, prong, homing wall, homing end, ring.
imp() { # WEAPON DISC NAME EXTRA_PRESSES [RUN_ARGS]: R1 that many times, Square at 600, the pad setup in $extra
  local w=$1 b=$2 n=$3 pre=$4 i
  local P="$(boot 1 $A)"
  if [ $w -gt 0 ]; then for i in $(seq 1 $w); do P="$P,$(at 1 $((100+i*50))):r1:10"; done; fi
  [ -n "$pre" ] && P="$P,$(at 1 400):$pre"
  P="$P,$(at 1 600):square:10"
  python3 -I $root/tools/x360_run.py cd $O/$n --build $b --press "$P" --steps 2000000000 --shots 10 "${extra[@]}" --watch gamecycle weapontype shotsfired testimp testimp+4 testimp+8 testimp+12 testimp+16 >/dev/null 2>&1
}
for setup in digital dpad stick; do
  extra=(); [ $setup != digital ] && extra=(--analog)
  L="left:33"; [ $setup = stick ] && L="lstick=0/128:33"
  imp 0 $B6 imm-$setup ""			# the missile, in the pocket
  imp 3 $B6 imp-pro-$setup "$L"		# the proton ball, in the pocket: ends after 350 cycles
  imp 6 $B6 imp-homw-$setup ""		# the homing shot, in the pocket: ends at a wall
  imp 7 $B6 imp-ring-$setup ""		# the ring, in the pocket: its centres hit the walls
  imp 5 $B5 imp-prong-$setup ""		# the prong, in the open: ends when its time is up
  imp 6 $B5 imp-home-$setup ""		# the homing shot, in the open with no target: ends when its time is up
  m0=$(mx $O/imm-$setup testimp); m1=$(mx $O/imm-$setup testimp+4); m2=$(mx $O/imm-$setup testimp+8); m3=$(mx $O/imm-$setup testimp+12); m4=$(mx $O/imm-$setup testimp+16)
  check "[$setup] The missile adds none of the PC's impact pictures (counts $m0 $m1 $m2 $m3 $m4; it has the Saturn's explosions)" $([ "$(mx $O/imm-$setup shotsfired)" -ge 1 ] && [ "$((m0+m1+m2+m3+m4))" = 0 ] && echo 1 || echo 0)
  a=$(mx $O/imp-pro-$setup testimp); w=$(mx $O/imp-pro-$setup weapontype); f=$(mx $O/imp-pro-$setup shotsfired)
  check "[$setup] The proton ball (weapon $w, $f shot) ends and adds its impact picture ($a)" $([ "$w" = 3 ] && [ "$a" -ge 1 ] && echo 1 || echo 0)
  a=$(mx $O/imp-homw-$setup testimp+8); w=$(mx $O/imp-homw-$setup weapontype)
  check "[$setup] The homing shot (weapon $w) hits a wall and adds its impact picture ($a)" $([ "$w" = 6 ] && [ "$a" -ge 1 ] && echo 1 || echo 0)
  a=$(mx $O/imp-ring-$setup testimp+16); w=$(mx $O/imp-ring-$setup weapontype)
  check "[$setup] The ring (weapon $w) hits a wall and adds its impact picture ($a)" $([ "$w" = 7 ] && [ "$a" -ge 1 ] && echo 1 || echo 0)
  a=$(mx $O/imp-prong-$setup testimp+4); w=$(mx $O/imp-prong-$setup weapontype)
  check "[$setup] The prong (weapon $w) ends and adds its impact picture ($a)" $([ "$w" = 5 ] && [ "$a" -ge 1 ] && echo 1 || echo 0)
  a=$(mx $O/imp-home-$setup testimp+12); b2=$(mx $O/imp-home-$setup testimp+8); w=$(mx $O/imp-home-$setup weapontype)
  check "[$setup] The homing shot with no target and no wall ends when its time is up and adds its picture ($a; none for a wall: $b2)" $([ "$w" = 6 ] && [ "$a" -ge 1 ] && [ "$b2" = 0 ] && echo 1 || echo 0)
done
echo "frames: $O/*/tick-*.ppm"
exit $((fails>0))
