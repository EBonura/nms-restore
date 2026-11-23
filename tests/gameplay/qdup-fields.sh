#!/bin/zsh
# QD Up's option screen draws both of its fields (docs/DECISIONS.md, "The option screen draws both of its fields").
#   tests/gameplay/qdup-fields.sh BUILD_DIR OUT_DIR GATE
# The option screen is one 320 x 480 interlaced picture in one buffer, drawn with dfe 0: while a field is on show the GPU
# draws only the other field's lines, so one pass reaches one field, and the passes must take turns (GFX.C display_screen,
# the layer's VSyncField). GATE (the harness's {tool:gate}) runs tests/gameplay/qdup-fields.toml on the disc in BUILD_DIR,
# on the hardware model PSOXIDE_HARDWARE names, and saves the 320 x 480 picture as VRAM holds it at each checkpoint: the
# title (PRESS START), the option screen after three changes (two checkpoints one field apart each), the 2D or 3D choice
# and PLEASE WAIT. A field is the picture's even lines or its odd lines. The checks:
#  - neither field of any option screen frame still holds the title picture (more than 40 percent of its pixels differ
#    from the same field of the title frame by more than 48 in a channel): both fields carry the option screen;
#  - after each change, the even lines of one frame agree with the odd lines of the other, or its odd lines with the
#    other's even lines (at most 8 percent differ; the rotating icon models move a little from one pass to the next, and a
#    frame may catch the field being drawn half done);
#  - the 2D or 3D choice and PLEASE WAIT, drawn and then left alone, hold one picture on both fields: the even lines agree
#    with the odd lines of the same frame (at most 8 percent; PLEASE WAIT in the 256 lines the game shows while it loads).
# A field that is never redrawn keeps the title picture, PRESS START or a closed panel and fails them; the test build
# QDUP_TEST_ONE_FIELD (every pass in the same field) fails them.
# Prints PASS or FAIL per check; exit status 1 if any failed. Frames in OUT_DIR/qdup-fields/<checkpoint>.new.png.
root=$(cd "$(dirname "$0")/../.." && pwd)
B=$1; O=$2; G=$3
mkdir -p $O
fails=0
check() { if [ "$2" = 1 ]; then echo "PASS  $1"; else echo "FAIL  $1"; fails=$((fails+1)); fi }
# the journey with BUILD_DIR's disc and link map in place of build/qdup-cd (absolute: the gate reads them from the journey)
Ba=$(cd $B && pwd)
sed "s#build/qdup-cd/#$Ba/#" $root/tests/gameplay/qdup-fields.toml >| $O/qdup-fields.toml
$G run $O/qdup-fields.toml --no-hw --out $O >| $O/gate.txt 2>&1
grep -q "^PASS qdup-fields" $O/gate.txt
check "the run reaches the option screen, changes the caption, closes the panel, opens the choice and starts loading ($O/gate.txt)" $([ $? = 0 ] && echo 1 || echo 0)
# fields.py MODE FRAME... prints percentages (100 when a frame cannot be read):
#   title T F: F's even field against T's even field, F's odd against T's odd
#   pair F0 F1: F0's even lines against F1's odd lines, F0's odd against F1's even
#   self F ROWS: F's even lines against its odd lines, in its first ROWS rows
fields() {
python3 -I - "$@" <<'PY'
import struct,sys,zlib
def png(f):  # 8-bit RGB or RGBA PNG, as the gate writes its frames
    d=open(f,'rb').read(); i=8; idat=b''; w=h=ct=0
    while i<len(d):
        n,t=struct.unpack('>I4s',d[i:i+8]); c=d[i+8:i+8+n]; i+=12+n
        if t==b'IHDR': w,h,_,ct=struct.unpack('>IIBB',c[:10])
        elif t==b'IDAT': idat+=c
    bpp=4 if ct==6 else 3; s=w*bpp; raw=zlib.decompress(idat); rows=[]; prev=bytearray(s)
    for y in range(h):
        ft=raw[y*(s+1)]; cur=bytearray(raw[y*(s+1)+1:(y+1)*(s+1)])
        for x in range(s):
            a=cur[x-bpp] if x>=bpp else 0; b=prev[x]; c=prev[x-bpp] if x>=bpp else 0
            if ft==1: cur[x]=(cur[x]+a)&255
            elif ft==2: cur[x]=(cur[x]+b)&255
            elif ft==3: cur[x]=(cur[x]+(a+b)//2)&255
            elif ft==4:
                p=a+b-c; pa,pb,pc=abs(p-a),abs(p-b),abs(p-c)
                cur[x]=(cur[x]+(a if pa<=pb and pa<=pc else b if pb<=pc else c))&255
        rows.append(bytes(cur[k] for k in range(s) if k%bpp<3)); prev=cur
    return w,rows
def diff(r0,r1,w):
    n=0
    for a,b in zip(r0,r1):
        n+=sum(1 for k in range(0,w*3,3) if max(abs(a[k]-b[k]),abs(a[k+1]-b[k+1]),abs(a[k+2]-b[k+2]))>48)
    return 100.0*n/(w*min(len(r0),len(r1)))
try:
    m=sys.argv[1]
    if m=='title':
        w,t=png(sys.argv[2]); _,f=png(sys.argv[3])
        print('%.1f %.1f'%(diff(f[0::2],t[0::2],w),diff(f[1::2],t[1::2],w)))
    elif m=='pair':
        w,p=png(sys.argv[2]); _,q=png(sys.argv[3])
        print('%.1f %.1f'%(diff(p[0::2],q[1::2],w),diff(p[1::2],q[0::2],w)))
    else:
        w,p=png(sys.argv[2]); p=p[:int(sys.argv[3])]
        print('%.1f'%diff(p[0::2],p[1::2],w))
except (OSError,ValueError,IndexError,zlib.error):
    print('100 100')
PY
}
F=$O/qdup-fields
for s in entry caption closed; do
  case $s in
    entry)   what="the first frames of the option screen";;
    caption) what="the caption changed (START to OPTIONS)";;
    closed)  what="the NOT AVAILABLE panel closed";;
  esac
  for k in 0 1; do
    d=(${=$(fields title $F/title.new.png $F/$s-$k.new.png)})
    ok=$(/usr/bin/awk -v a=${d[1]:-0} -v b=${d[2]:-0} 'BEGIN{print (a>40 && b>40)?1:0}')
    check "$what, frame $k: neither field holds the title picture (even field ${d[1]:-?}%, odd field ${d[2]:-?}% of pixels differ from the title's)" $ok
  done
  d=(${=$(fields pair $F/$s-0.new.png $F/$s-1.new.png)})
  ok=$(/usr/bin/awk -v a=${d[1]:-100} -v b=${d[2]:-100} 'BEGIN{print (a<=8 || b<=8)?1:0}')
  check "$what: the fields agree (even lines of one frame against odd lines of the next: ${d[1]:-?}% and ${d[2]:-?}% of pixels differ)" $ok
done
for s in choice wait; do
  case $s in
    choice) what="the 2D or 3D choice, drawn and left alone"; rows=480;;
    wait)   what="PLEASE WAIT while the game loads (the 256 lines on show)"; rows=256;;
  esac
  d=$(fields self $F/$s.new.png $rows)
  ok=$(/usr/bin/awk -v a=${d:-100} 'BEGIN{print (a<=8)?1:0}')
  check "$what: both fields hold it (even lines against odd lines: ${d:-?}% of pixels differ)" $ok
done
# every option screen the options journey (tests/qdup-options.toml) visits, with the chevrons on the adjustable rows: two
# frames one pass apart each, and the even lines of one agree with the odd lines of the other (at most 8 percent differ)
sed "s#build/qdup-cd/#$Ba/#" $root/tests/qdup-options.toml >| $O/qdup-options.toml
$G run $O/qdup-options.toml --no-hw --out $O >| $O/gate-options.txt 2>&1
grep -q "^PASS qdup-options" $O/gate-options.txt
check "the options journey walks every row and value of both pages ($O/gate-options.txt)" $([ $? = 0 ] && echo 1 || echo 0)
for s in start options players-2players players-computer table-uk table-us table-coffin table-square balls-ukpool balls-us8 balls-9ball baize-blue baize-green baize-red speaker back; do
  d=(${=$(fields pair $O/qdup-options/$s-0.new.png $O/qdup-options/$s-1.new.png)})
  ok=$(/usr/bin/awk -v a=${d[1]:-100} -v b=${d[2]:-100} 'BEGIN{print (a<=8 || b<=8)?1:0}')
  check "option screen row $s: the fields agree (${d[1]:-?}% and ${d[2]:-?}% of pixels differ)" $ok
done
exit $((fails>0))
