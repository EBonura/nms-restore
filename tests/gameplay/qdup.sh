#!/bin/zsh
# QD Up gameplay check: the one disc in both views, from the boot, through the pad the emulator presents.
#   tests/gameplay/qdup.sh BUILD_DIR OUT_DIR
# Three setups: digital pad; DualShock in analog mode with the direction pad; the same with the left stick
# for every direction. Per view (3D, 2D) and setup: title and option screen to START; START opens the
# choice screen (the picked view is the one the game runs: view_3d); Left and Right change the cue angle
# (opposite ways); Cross held with Down draws the cue back and Up strikes; the balls move; the shot ends and
# the turn passes. Then, per view and for the digital pad and the stick, a second run checks the choice
# screen's own buttons (Down moves to 2D FLAT, Up back to 3D TABLE, Circle goes back to the option screen
# and START opens it again) and, in the game, L2 and R2 together: the view switches and the game carries on
# (Left still turns the aim, before and after each switch), then back. Prints PASS or FAIL per check; exit
# status 1 if any failed.
root=$(cd "$(dirname "$0")/../.." && pwd)
B=$1; O=$2
mkdir -p $O
fails=0
# The shots start from the moment the game does, not from a tick number: anchor() runs the presses that
# start the game and finds the first tick after tick 100 that the cue has an angle (the game's state once it is running; what memory holds before the game has run depends on the build), and
# t N gives tick N of the script's original timing (the game started at tick ANCHOR_REF) counted from there.
ANCHOR_REF=2354
anchor() { # build dir, out dir, boot presses, extra emulator flags
  python3 -I $root/tools/qdup_run.py cd $2/anchor --build $1 --press "$3" --steps 2000000000 ${@:4} >/dev/null 2>&1
  A=$(/usr/bin/awk -F, '{sub(/\r$/,"")} NR==1{for(i=1;i<=NF;i++)if($i=="cue_angle")c=i} NR>1&&$1>100&&$c!=0{print $1; exit}' $2/anchor/vars.csv)
  [ -n "$A" ] || { echo "FAIL  the game never started in the boot run"; exit 1; }
}
t() { echo $(( A + $1 - ANCHOR_REF )); }
val() { /usr/bin/awk -F, -v n=$2 -v t=$3 '{sub(/\r$/,"")} NR==1{for(i=1;i<=NF;i++)if($i==n)c=i} NR>1&&$1==t{print $c; exit}' $1/vars.csv; }
maxv() { /usr/bin/awk -F, -v n=$2 -v a=$3 -v b=$4 '{sub(/\r$/,"")} NR==1{for(i=1;i<=NF;i++)if($i==n)c=i} NR>1&&$1>=a&&$1<=b{x=$c+0; if(x<0)x=-x; if(x>m)m=x} END{print m+0}' $1/vars.csv; }
check() { if [ "$3" = 1 ]; then echo "PASS  $1  $2"; else echo "FAIL  $1  $2"; fails=$((fails+1)); fi }
for view in 3d 2d; do
  [ $view = 3d ] && want=1 || want=0
  for setup in digital dpad stick; do
    case $setup in
      digital) extra=();         U="up"; D="down"; L="left"; R="right";;
      dpad)    extra=(--analog); U="up"; D="down"; L="left"; R="right";;
      stick)   extra=(--analog); U="lstick=128/0"; D="lstick=128/255"; L="lstick=0/128"; R="lstick=255/128";;
    esac
    P="1000:start:10,1150:$U:6,1210:$U:6,1270:$R:6,1330:$U:6,1390:$U:6,1450:$D:6,1510:$D:6,1570:$D:6,1630:$D:6,1750:cross:6"
    if [ $view = 2d ]; then P="$P,1850:$D:6,1950:cross:6"; else P="$P,1950:cross:6"; fi   # the choice screen: 3D TABLE is picked at first
    S="$view/$setup"; V=$O/$view-$setup
    mkdir -p $V
    anchor $B $V "$P" $extra
    # The strike is at tick 3960 (the script's original 3966, six earlier). The shot's outcome is bistable in the
# strike's phase: a strike a few ticks later ends the game's frame (draw_end_of_frame, "FRAME REPLAY"), whose
# len_text scan costs about 5M cycles a frame and halves the flips of the run. Measured on mister,gpu-irq=never,
# flips of the 3D digital route against the strike offset from tick 3966 (offset -6 -3 0 +3 +6 +9 +12 +15):
# SDK 1c3133be 2914 2914 2914 1208 1219 1399 2853 2940, SDK 210b5695 2918 2918 1208 1213 1410 2857 2944 2963.
# Both sit on the plateau at -6 and -3 and are three ticks or more from either cliff there.
P="$P,$(t 3600):$L:13,$(t 3650):$R:13,$(t 3800):cross:300,$(t 3840):$D:120,$(( $(t 3966) - 6 )):$U:100"
    python3 -I $root/tools/qdup_run.py cd $V --build $B --press "$P" --steps 2400000000 --shots 400 --watch Player_Active --watch event_flag --watch view_3d $extra >/dev/null 2>&1
    check $S "the choice screen set the view (view_3d $(val $V view_3d $(t 3700)), want $want)" $([ "$(val $V view_3d $(t 3700))" = $want ] && echo 1 || echo 0)
    a0=$(val $V cue_angle $(t 3595)); a1=$(val $V cue_angle $(t 3645)); a2=$(val $V cue_angle $(t 3695))
    check $S "Left changes the cue angle (cue_angle $a0 -> $a1)" $([ -n "$a1" ] && [ "$a1" != "$a0" ] && echo 1 || echo 0)
    check $S "Right changes it the other way (cue_angle $a1 -> $a2, left made $((a1-a0)), right made $((a2-a1)))" $([ $(( (a1-a0)*(a2-a1) )) -lt 0 ] && echo 1 || echo 0)
    check $S "Cross held with Down draws the cue back (mouse_ydiff max $(maxv $V mouse_ydiff $(t 3840) $(t 3960)))" $([ $(maxv $V mouse_ydiff $(t 3840) $(t 3960)) -gt 0 ] && echo 1 || echo 0)
    check $S "Up strikes, the balls move (ball0 speed max $(maxv $V ball0_vx $(t 3967) $(t 4700)))" $([ "$(/usr/bin/awk -v v=$(maxv $V ball0_vx $(t 3967) $(t 4700)) 'BEGIN{print (v>0)}')" = 1 ] && echo 1 || echo 0)
    # closed on the game's state: the first tick after the strike at which the player to play has changed, within 3,000
    # ticks (the shot's length depends on where the balls go; a fixed tick, 4500, failed when a shot ran 100 ticks longer)
    p0=$(val $V Player_Active $(t 3700))
    tp=$(/usr/bin/awk -F, -v n=Player_Active -v a=$(t 3967) -v b=$(t 6967) -v p=$p0 '{sub(/\r$/,"")} NR==1{for(i=1;i<=NF;i++)if($i==n)c=i} NR>1&&$1>=a&&$1<=b&&$c!=p{print $1; exit}' $V/vars.csv)
    p1=$([ -n "$tp" ] && val $V Player_Active $tp)
    check $S "The shot ends and the turn passes (Player_Active $p0 -> ${p1:-$p0}, at tick ${tp:-never}, strike at $(t 3967))" $([ -n "$tp" ] && echo 1 || echo 0)
  done
  # the choice screen's buttons and the switch in the game, digital pad and the analog stick
  for setup in digital stick; do
    case $setup in
      digital) extra=();         U="up"; D="down"; L="left"; R="right";;
      stick)   extra=(--analog); U="lstick=128/0"; D="lstick=128/255"; L="lstick=0/128"; R="lstick=255/128";;
    esac
    P="1000:start:10,1150:$U:6,1210:$U:6,1270:$R:6,1330:$U:6,1390:$U:6,1450:$D:6,1510:$D:6,1570:$D:6,1630:$D:6,1750:cross:6"
    P="$P,1850:$D:6,1950:$U:6"                  # the choice screen: Down moves to 2D FLAT, Up back to 3D TABLE
    P="$P,2050:circle:6,2100:$U:6,2200:$D:6"    # Circle goes back to the option screen (Up then moves its cursor), Down returns to START
    P="$P,2300:cross:6,2400:$D:6"               # START opens the choice screen again; Down: 2D FLAT
    if [ $view = 3d ]; then P="$P,2500:$U:6"; fi
    P="$P,2600:cross:6"                          # confirm: the game runs in $view
    S="$view/$setup/switch"; V=$O/$view-$setup-switch
    mkdir -p $V
    anchor $B $V "$P" $extra
    ANCHOR_REF=2354
    P="$P,$(t 3650):$L:20,$(t 3850):l2:6,$(t 3850):r2:6,$(t 4000):$L:20,$(t 4200):l2:6,$(t 4200):r2:6,$(t 4350):$L:20"
    python3 -I $root/tools/qdup_run.py cd $V --build $B --press "$P" --steps 2400000000 --shots 400 --watch view_3d --watch cursor $extra >/dev/null 2>&1
    check $S "Down on the choice screen picks 2D (view_3d $(val $V view_3d 1900))" $([ "$(val $V view_3d 1900)" = 0 ] && echo 1 || echo 0)
    check $S "Up picks 3D again (view_3d $(val $V view_3d 2000))" $([ "$(val $V view_3d 2000)" = 1 ] && echo 1 || echo 0)
    c0=$(val $V cursor 2090); c1=$(val $V cursor 2150)
    check $S "Circle goes back to the option screen (its cursor then moves with Up: $c0 -> $c1)" $([ -n "$c1" ] && [ "$c0" != "$c1" ] && echo 1 || echo 0)
    check $S "START opens the choice screen again and it remembers 3D (view_3d $(val $V view_3d 2350)), Down picks 2D ($(val $V view_3d 2450))" $([ "$(val $V view_3d 2350)" = 1 ] && [ "$(val $V view_3d 2450)" = 0 ] && echo 1 || echo 0)
    [ $view = 3d ] && vw=1 || vw=0
    check $S "the game runs in the picked view (view_3d $(val $V view_3d $(t 3600)), want $vw)" $([ "$(val $V view_3d $(t 3600))" = $vw ] && echo 1 || echo 0)
    v1=$(val $V view_3d $(t 3950)); v2=$(val $V view_3d $(t 4300))
    check $S "L2 and R2 together switch the view and back (view_3d $vw -> $v1 -> $v2)" $([ "$v1" != "$vw" ] && [ "$v2" = "$vw" ] && echo 1 || echo 0)
    g0=$(val $V cue_angle $(t 3645)); g1=$(val $V cue_angle $(t 3750)); h0=$(val $V cue_angle $(t 3995)); h1=$(val $V cue_angle $(t 4100)); k0=$(val $V cue_angle $(t 4345)); k1=$(val $V cue_angle $(t 4450))
    check $S "the game carries on in each view: Left still turns the aim before ($g0 -> $g1), after the first switch ($h0 -> $h1) and after the second ($k0 -> $k1)" $([ "$g0" != "$g1" ] && [ "$h0" != "$h1" ] && [ "$k0" != "$k1" ] && echo 1 || echo 0)
  done
done
echo "frames: $O/<view>-<setup>/tick-*.ppm every 400 ticks"
exit $((fails>0))
