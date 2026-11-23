#!/bin/zsh
# 360X craft rules restored from NMS's PC and Saturn versions, on the disc, through the pad.
#   tests/gameplay/x360-craft.sh BUILD_DIR OUT_DIR TOUCH_DIR
# TOUCH_DIR is a disc built with X360_CFLAGS=-DX360_TEST_TOUCH (the still check reads which creature the craft last touched, and when).
# fuel: thrust still works with an empty gauge, the gauge falls at the PC's rate.
# edges: the craft stays on a 960 x 960 map (Mission 1) and a 1280 x 720 map (Mission 3), thrusting up, left, down and right.
# start: each of the nine missions starts the craft at its place, on an empty pixel of the collision map.
# terrain: no pixel of the craft's shape is on a solid pixel (Mission 1, two scripts), and leaves its start in every mission (a third script).
# Prints PASS or FAIL per check; exit status 1 if any failed. Frames are in OUT_DIR.
# Every input script starts from the moment its mission starts, not from a tick number: anchor K runs the boot
# presses to Mission K alone and finds the first tick the craft has fuel (Init_Level sets it); `at K N` is tick N
# counted from there (the other scripts do the same with the mission's start at tick 2664).
root=$(cd "$(dirname "$0")/../.." && pwd)
B=$1; O=$2; T=$3
mkdir -p $O
fails=0
data=${X360_DATA:-$root/build/originals/360x}
# boot presses that reach mission K (1 to 9) and then start it (the first skips the boot splash, so the screens after
# it are the ones the timed presses below were written for)
boot() { # K [LIMIT]: with LIMIT, no Start press at or after tick LIMIT-100 (a Start after the mission began would pause it)
  python3 -I -c "
k=$1; lim=int('${2:-999999}')-100
p=['100:start','150:start','350:start','550:start','750:start']
for i in range(k-1): p.append(f'{1000+40*i}:right')
t=1000+40*max(k-1,0)+100
if t<lim: p.append(f'{t}:start')
for j in range(1,5):
    if t+200*j<lim: p.append(f'{t+200*j}:start')
s=int('${X360_BOOT_SHIFT:-0}')   # every press moved by this many ticks (X360_BOOT_SHIFT, 0 unless a sweep asks): another random sequence
print(','.join(str(int(x.split(':')[0])+s)+':'+x.split(':',1)[1] for x in p))"
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

# >>> fuel
# --- fuel: Mission 1, Cross held for 4200 ticks from 340 ticks after the start of play (about 1700 game cycles, more
# than the 1088 thrust frames the PC's rate gives a full tank)
anchor 1
D=$O/fuel
P="$(boot 1 ${AK[1]}),$(at 1 340):cross:4200"
python3 -I $root/tools/x360_run.py cd $D --build $B --press "$P" --steps 3000000000 --shots 400 --watch gamecycle fuel boost crafty craftx >/dev/null 2>&1
t0=$(first $D fuel 0 $(at 1 440))
f1=$(val $D fuel $(at 1 740))
b0=""; b1=""
if [ -n "$t0" ]; then b0=$(val $D boost $((t0+300))); b1=$(val $D boost $((t0+1000))); fi
check "The gauge falls slower than one step per thrust frame (fuel $(val $D fuel $(at 1 440)) -> $f1 in 300 ticks of thrust, 192 at the start)" $([ -n "$f1" ] && [ "$f1" -gt 150 ] && [ "$f1" -lt 192 ] && echo 1 || echo 0)
check "The gauge empties with Cross held (fuel 0 at tick ${t0:-never})" $([ -n "$t0" ] && echo 1 || echo 0)
check "Thrust still runs with an empty gauge (boost $b0 at tick $((t0+300)), $b1 at $((t0+1000)), fuel $(val $D fuel $((t0+1000))))" $([ "$b0" = 4 ] && [ "$b1" = 4 ] && echo 1 || echo 0)
# <<< fuel

# >>> edges
# --- edges: thrust 600 ticks in each of four headings (the craft turns a quarter left each time), on the map sizes of
# Mission 1 (960 x 960) and Mission 3 (1280 x 720); the craft's position must stay inside the map
ext() { # vars.csv, first tick: min and max of craftx and crafty from that tick on, 32-bit values read as signed
  /usr/bin/awk -F, -v a=$2 '{sub(/\r$/,"")} NR==1{for(i=1;i<=NF;i++){if($i=="craftx")cx=i; if($i=="crafty")cy=i}} NR>1&&$1>=a&&$cx!=0{x=$cx; y=$cy; if(x>2147483647)x-=4294967296; if(y>2147483647)y-=4294967296; if(n==0||x<x0)x0=x; if(n==0||x>x1)x1=x; if(n==0||y<y0)y0=y; if(n==0||y>y1)y1=y; n++} END{print x0,x1,y0,y1}' $1/vars.csv; }
mw=(960 1280); mh=(960 720); mm=(1 3)
for i in 1 2; do
  k=${mm[$i]}; D=$O/edges$k
  anchor $k
  P="$(boot $k ${AK[$k]}),$(at $k 340):left:40,$(at $k 400):cross:600,$(at $k 1040):left:40,$(at $k 1100):cross:600,$(at $k 1740):left:40,$(at $k 1800):cross:600,$(at $k 2440):left:40,$(at $k 2500):cross:600"
  python3 -I $root/tools/x360_run.py cd $D --build $B --press "$P" --steps 3000000000 --shots 200 --watch gamecycle angle craftx crafty >/dev/null 2>&1
  e=($(ext $D $(at $k 100)))
  check "Mission $k (${mw[$i]} x ${mh[$i]}): the craft stays on the map (x ${e[1]} to ${e[2]}, y ${e[3]} to ${e[4]})" $([ "${e[1]}" -ge 0 ] && [ "${e[2]}" -le ${mw[$i]} ] && [ "${e[3]}" -ge 0 ] && [ "${e[4]}" -le ${mh[$i]} ] && echo 1 || echo 0)
done
# <<< edges

# >>> start

# The craft's shape on the map: shape fit K X Y     the number of its 64 frames that are on no solid pixel at X, Y
#                               shape hits K CSV T  the number of ticks from tick T at which the shape (frame craftn, position craftx, crafty)
#                                                    is on a solid pixel. The shape is made here from NMS's own S_SHIP frames
#                                                    by the rule in SPRITE2.C (a line runs from its first to its last pixel that is not
#                                                    clear) and put where Print_Sprites draws the craft (x + u, y + h/2 + 2 + v), so it
#                                                    is a second implementation of what the game does, not a read-back of it.
shape() {
  python3 -I - "$@" <<PY
import csv,struct,sys,importlib.util
spec=importlib.util.spec_from_file_location('s2s','$root/tools/x360_scs2spr.py')
m=importlib.util.module_from_spec(spec); spec.loader.exec_module(m)
frames,_=m.read_ps1('$data/MISC/S_SHIP.PSX','$data/MISC/S_SHIP.SPR')
off=frames[0]['h']//2+2
rows=[[(min(i for i,p in enumerate(r) if p),max(i for i,p in enumerate(r) if p)) if any(r) else None for r in f['pix']] for f in frames]
mode,k=sys.argv[1],int(sys.argv[2])
d='$data/MAPS/'
w=struct.unpack('<H',open(d+'LEV%d.MAP'%k,'rb').read(2))[0]*16
h=struct.unpack('<H',open(d+'LEV%d.MAP'%k,'rb').read(4)[2:4])[0]*16
c=open(d+'LEVEL%dC.BIN'%k,'rb').read()
cw=len(c)*8//h   # the collision map's own width (missions 7-9: 980 of a 992 map; SCROLLER.C colwidth)
def solid(x,y):
    if x<0 or x>=cw: return 1
    p=x+y*cw
    return p>=0 and (p>>3)<len(c) and (c[p>>3]>>(7-(p&7)))&1
def hit(x,y,fr):
    f=frames[fr]
    for iy,rw in enumerate(rows[fr]):
        yy=y+off+f['v']+iy
        if rw is None or not (0<yy<h): continue
        for xx in range(x+f['u']+rw[0],x+f['u']+rw[1]+1):
            if solid(xx,yy): return True
    return False
if mode=='fit':
    x,y=int(sys.argv[3]),int(sys.argv[4])
    print(sum(1 for fr in range(64) if not hit(x,y,fr)))
else:
    t0=int(sys.argv[4]); n=0; last=None
    for r in csv.DictReader(open(sys.argv[3]+'/vars.csv')):
        if int(r['tick'])<t0 or int(r['craftx'])==0: continue
        x=int(r['craftx']); y=int(r['crafty']); fr=int(r['craftn'])&63
        x=x-2**32 if x>=2**31 else x
        y=y-2**32 if y>=2**31 else y
        # a sample can land between the turn that sets craftn and the revert that follows it in the same
        # cycle, so only a state seen in two samples in a row counts as the craft being in the wall
        bad=hit(x,y,fr) and last==(x,y,fr)
        last=(x,y,fr) if hit(x,y,fr) else None
        if bad: n+=1
    print(n)
PY
}
# --- start: the first position the craft has in each mission (CONTROL.C startx, starty) and the collision map there
for k in 1 2 3 4 5 6 7 8 9; do
  D=$O/start$k
  anchor $k
  python3 -I $root/tools/x360_run.py cd $D --build $B --press "$(boot $k ${AK[$k]})" --steps 1100000000 --shots 100 --watch gamecycle level craftx crafty pause >/dev/null 2>&1
  row=$(/usr/bin/awk -F, -v a=${AK[$k]} '{sub(/\r$/,"")} NR>1&&$1>=a&&$4!=0{print $4","$5; exit}' $D/vars.csv)
  x=${row%,*}; y=${row#*,}
  fit=$(shape fit $k ${x:-0} ${y:-0})
  paused=$(/usr/bin/awk -F, -v a=$((${AK[$k]}+100)) '{sub(/\r$/,"")} $1==a{print $6}' $D/vars.csv)
  check "Mission $k starts the craft at ${sx[$k]}, ${sy[$k]}, where its shape fits in all 64 frames (first position ${x:-none}, ${y:-none}; shape fits in $fit of 64 frames; paused ${paused:-?} 100 ticks after the start)" $([ "$x" = "${sx[$k]}" ] && [ "$y" = "${sy[$k]}" ] && [ "$fit" = 64 ] && [ "$paused" = 0 ] && echo 1 || echo 0)
done
# <<< start

# >>> still
# --- still: Mission 6 with no input after the start presses, 1,200 game cycles. What it proves: nothing takes shields from a craft that
# nothing touched. With no input the only thing that lowers the shields is a creature touching the craft (CONTROL.C, 2 points, then 16
# cycles of flashing), and the game's random sequence (never seeded, as in the PC and Saturn sources) decides whether a drifting rock or a
# spitter comes to a craft that does not move, so "the shields stay full" proved only one sequence (the old start, 676,434, lost 2 points
# every 16 cycles to two spitters and the craft was destroyed at cycle 1009; and main's own run fails for a boot 36 ticks later). So the
# check is: the run lasts its 1,200 cycles with the craft alive, and every fall of the shields falls in a game cycle in which the build
# recorded a creature touch (testtouch: the last creature touched and its cycle); a fall with no touch fails, for any sequence.
anchor 6
D=$O/still6
python3 -I $root/tools/x360_run.py cd $D --build $T --press "$(boot 6 ${AK[6]})" --steps 2600000000 --shots 100 --watch gamecycle shields craftdestroyed testtouch testtouch+12 >/dev/null 2>&1
read lastgc losses unmatched dead <<< $(/usr/bin/awk -F, -v a=$((${AK[6]}+10)) '{sub(/\r$/,"")} NR==1{for(i=1;i<=NF;i++){if($i=="gamecycle")g=i; if($i=="shields")s=i; if($i=="craftdestroyed")d=i; if($i=="testtouch")n=i; if($i=="testtouch+12")c=i}} NR>1&&$1>=a&&$s!=0&&$g<=1200{ if($g>lg)lg=$g; if($d>dd)dd=$d; if(ps!=""&&$s<ps){ l++; dg=$g-$c; if(!($n!=0&&dg>=-1&&dg<=1)) u++ } ps=$s } END{print lg+0, l+0, u+0, dd+0}' $D/vars.csv)
check "Mission 6, standing still for $lastgc game cycles (1,200 asked): the craft is not destroyed ($dead)" $([ "$lastgc" -ge 1200 ] && [ "$dead" = 0 ] && echo 1 || echo 0)
check "Mission 6, standing still: every fall of the shields ($losses) coincides with a recorded creature touch (without one: $unmatched)" $([ "$unmatched" = 0 ] && echo 1 || echo 0)
# <<< still

# >>> terrain
# --- terrain: the craft cannot pass through a wall, and leaves its start in every mission
# ticks from tick $3 on at which the craft's shape is on a solid pixel (mission $2): shape hits
solidn() { shape hits $2 $1 $3; }
# farthest the craft gets from its start from tick $4 on (distance in pixels, rounded down)
far() {
  python3 -I -c "
import csv
sx,sy=$2,$3
m=0
for r in csv.DictReader(open('$1/vars.csv')):
    if int(r['tick'])<$4 or int(r['craftx'])==0: continue
    x=int(r['craftx']); y=int(r['crafty'])
    x=x-2**32 if x>=2**31 else x
    y=y-2**32 if y>=2**31 else y
    m=max(m,int(((x-sx)**2+(y-sy)**2)**0.5))
print(m)"
}
# Thrust bursts of 20 to 70 ticks with turns and pauses, from a fixed seed (the same scripts in every mission)
burst() { # seed first_tick bursts
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
for seed in 7 11; do
  D=$O/wall$seed
  anchor 1
  P="$(boot 1 ${AK[1]}),$(burst $seed $(at 1 340) 40)"
  python3 -I $root/tools/x360_run.py cd $D --build $B --press "$P" --steps 1700000000 --shots 400 --watch gamecycle craftx crafty craftn >/dev/null 2>&1
  n=$(solidn $D 1 ${AK[1]})
  check "Mission 1, script $seed: the craft's shape is never inside a wall (solid pixel under it at $n of its positions; $(far $D 767 932 ${AK[1]}) pixels from its start at the farthest)" $([ "$n" = 0 ] && echo 1 || echo 0)
done
# A script's timing against the game's cycles shifts with any change in the code's size or speed, and a start in a narrow place can
# trap a random script (Mission 5's start did in one build: both random scripts stayed within 36 pixels). The second script is not random: a quarter turn and
# 160 ticks of thrust, ten times, so every heading is tried from where the craft is; it runs only if the first did not get 40 pixels away.
# The third, run only if neither got 40 pixels away, is what a player does first: thrust held from the start, where every life
# points up (since 2026-11-18). In Mission 1's funnel the two scripts' turns now wedge the craft (32 pixels at most, against 40
# or more when lives started thrusting right): it climbs out straight up (tests/x360-start-heading.toml).
sweep() { # first_tick
  python3 -I -c "
t=$1; out=[]
for i in range(10):
    out.append(f'{t}:left:22'); t+=40
    out.append(f'{t}:cross:160'); t+=190
print(','.join(out))"
}
for k in 1 2 3 4 5 6 7 8 9; do
  anchor $k
  f=0; n=0
  for script in burst sweep climb; do
    D=$O/leave${k}_$script
    case $script in
      burst) P="$(boot $k ${AK[$k]}),$(burst 7 $(at $k 340) 40)";;
      sweep) P="$(boot $k ${AK[$k]}),$(sweep $(at $k 340))";;
      climb) P="$(boot $k ${AK[$k]}),$(at $k 340):cross:300";;
    esac
    python3 -I $root/tools/x360_run.py cd $D --build $B --press "$P" --steps 1700000000 --shots 400 --watch gamecycle craftx crafty craftn >/dev/null 2>&1
    fs=$(far $D ${sx[$k]} ${sy[$k]} ${AK[$k]}); n=$((n+$(solidn $D $k ${AK[$k]})))
    [ "$fs" -gt "$f" ] && f=$fs
    [ "$f" -ge 40 ] && break
  done
  check "Mission $k: the craft leaves its start (up to $f pixels away) and its shape is never inside a wall (solid under it at $n positions)" $([ "$f" -ge 40 ] && [ "$n" = 0 ] && echo 1 || echo 0)
done
# <<< terrain

echo "frames: $O/<check>/tick-*.ppm"
exit $((fails>0))
