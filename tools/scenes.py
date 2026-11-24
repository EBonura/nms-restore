#!/usr/bin/env python3
"""Scene images of a restored game (R2: the program split into a resident base and images that load
into one shared region, docs/DECISIONS.md "Scene images").

    scenes.py ld      SCENES.TXT OUT.ld OBJDIR              the linker script for the images
    scenes.py wrap    SCENES.TXT OUT.s                      the wrapped entry points (GNU as)
    scenes.py check   SCENES.TXT ELF MAP OBJ... ARCHIVE     fail on a reference into an image that
                                                            may not be loaded
    scenes.py cut     SCENES.TXT ELF OUTDIR                 cut each image out of the link:
                                                            OUTDIR/SCENES.<NAME>.bin and .map
    scenes.py patch   EXE MAP BIN BINMAP TOOLDIR            patch an image's load-delay hazards
                                                            (the SDK's hazard-patch, through the
                                                            program's trampolines; EXE is updated)
    scenes.py pack    SCENES.TXT OUTDIR OUT.IMG [--list TXT]  SCENES.IMG (table sector, then images)
    scenes.py splice  SCENES.TXT OUTDIR EXE                 the preloaded image into the EXE
    scenes.py embed   SCENES.IMG EXE LBA                    the table's rows into the EXE, SCENES.IMG at LBA
    scenes.py flags   SCENES.TXT                            the linker's --wrap flags

SCENES.TXT, one directive a line, `#` starts a comment:

    core                       the program carries the SDK scene core (psx-core, sdk/psoxide-core.ld):
                               its stored copy right after .data, where the region and .bss will be
                               (`__core_boot` copies it into the window), the boot code's store after
                               it; the region's images are not carried (the core loads them)
    arrange overlap|resident   the per-game setting (owner, 2026-11-23: a modular loader, the
                               arrangement a setting): `overlap` puts the region images at one
                               address, loaded on demand (frees RAM, a read at each switch);
                               `resident` puts each at its own address, all carried by the EXE
                               (no read at a switch, the RAM of all of them). SCENES_ARRANGE in
                               the environment overrides it (measurements of both)
    region ALIGN OFFSET        the region starts at the first address after .data that is OFFSET
                               past a multiple of ALIGN (the I-cache knob: the region's code keeps
                               its place in the 4 KB cache whatever the base grows or shrinks to)
    image ID NAME [preload|boot]  a new image; ID is its scene id (its row in the table).
                               `preload` (one region image, the front end): the EXE carries it in
                               the region, so the BIOS loads it with the program and the first
                               screen needs no read. `boot` (one image, the splash and film): it
                               runs at the start of the heap, which takes it back later, as
                               psyq-boot.ld's overlay did; the EXE carries it after the preloaded
                               image and `__core_boot` (psyq/src/boot.rs) moves it up before
                               `_start`. It is valid until the first entry into another image;
                               its entries may run only before that (the program's start)
    objects NAME...            NMS objects (NAME.o) whose code and constant data go in the image
                               (.text*, .rodata*): their .data and .bss stay in the base, so a
                               reload never resets a variable
    functions NAME FN...       single functions of an object built with -ffunction-sections
                               (.text.FN of NAME.o)
    code NAME...               NMS objects whose code goes in the image and whose constants stay in
                               the base (data the base reads points at them)
    rodata NAME...             the constant data of objects whose functions are listed one by one
    except NAME FN...          every function of NAME.o (built with -ffunction-sections) but these,
                               which stay in the base, and its constant data
    layer PATTERN...           input sections of the layer (libpsyq.a) by section-name glob, any
                               kind: code, constants, data and bss alike (only BOOT, loaded once,
                               may hold layer data)
    image ... heap             (core, with a boot image) the image runs in the heap's bottom, right above
                               the boot code: the EXE carries it after the boot code and the boot mover
                               moves both, so the first screen needs no read. The game's heap takes the
                               memory back once another image runs (a volatile overlay of the table:
                               the core reads it again at every load); the image costs no resident RAM
    entry FN...                functions the base calls to enter the image: ld --wrap=FN sends
                               every call through __wrap_FN, which loads the image first
    allow FN IMAGE  # reason   the base function FN (or the whole object FN.o) may refer to IMAGE
                               directly (it runs only while IMAGE is loaded, for the reason given)

The check reads each object's relocations (readelf), the link map and the ELF's symbols, and treats
every linked input section as a node and every relocation (a call, a jump, an address taken, a
pointer in data) as an edge. For each image, whatever is reachable from the program's entry or from
another image's code without going through one of the image's wrapped entries can run while the
image is not loaded: none of it may refer into the image, unless an `allow` line names the base
function or object (the flow guarantees the image is loaded there). A wrapped entry referred to
from inside another image is an error too (it would load over the code that called it).

SCENES.IMG: sector 0 is the scene table in the core's format (scene-system CORE-ABI.md section 1:
64-byte header "PSXSCN01", 64-byte rows), then each image from a sector boundary. Row 0 is the
program itself (its LBA and size are the boot exe's, which only the disc tool knows: 0 here). Image
rows are kind 1; their LBA is counted from the start of SCENES.IMG, not the disc (the core's table
will carry disc LBAs once mkisopsx writes it; the layer adds the file's own LBA meanwhile).
"""
import fnmatch
import os
import re
import struct
import subprocess
import sys

NM = 'mipsel-none-elf-nm'
READELF = 'mipsel-none-elf-readelf'
OBJCOPY = 'mipsel-none-elf-objcopy'
SECTOR = 2048
BASE = 'BASE'


class Image:
    def __init__(self, sid, name):
        self.id, self.name = sid, name
        self.objects, self.functions, self.layer, self.entries, self.rodata = [], [], [], [], []
        self.excepts, self.code = [], []


def parse(path):
    global ARRANGE, CORE
    ARRANGE = 'overlap'
    CORE = False
    region = (0x1000, 0)
    images, allows = [], []
    for n, raw in enumerate(open(path), 1):
        line = raw.split('#', 1)[0].split()
        if not line:
            continue
        kind, args = line[0], line[1:]
        if kind == 'core':
            CORE = True
        elif kind == 'arrange':
            ARRANGE = args[0]
        elif kind == 'region':
            region = (int(args[0], 0), int(args[1], 0))
        elif kind == 'image':
            images.append(Image(int(args[0]), args[1]))
            images[-1].preload = 'preload' in args[2:]
            images[-1].boot = 'boot' in args[2:]
            images[-1].heap = 'heap' in args[2:]
            if sum(i.boot for i in images) > 1:
                sys.exit(f'{path}:{n}: one preloaded image and one boot image at most')
        elif kind == 'objects':
            images[-1].objects += args
        elif kind == 'functions':
            images[-1].functions += [(args[0], f) for f in args[1:]]
        elif kind == 'code':
            images[-1].code += args
        elif kind == 'except':
            images[-1].excepts.append((args[0], args[1:]))
            images[-1].rodata.append(args[0])
        elif kind == 'rodata':
            images[-1].rodata += args
        elif kind == 'layer':
            images[-1].layer += args
        elif kind == 'entry':
            images[-1].entries += args
        elif kind == 'allow':
            allows.append((args[0], args[1]))
        else:
            sys.exit(f'{path}:{n}: unknown directive {kind}')
    ARRANGE = os.environ.get('SCENES_ARRANGE', ARRANGE)
    if ARRANGE not in ('overlap', 'resident'):
        sys.exit(f'{path}: arrange is overlap or resident')
    if ARRANGE == 'resident':
        for im in images:
            if not im.boot and not im.heap:
                im.preload = True
    if any(im.heap for im in images) and not (CORE and any(im.boot for im in images)):
        sys.exit(f'{path}: a heap image needs the core and a boot image (it is moved up with the boot code)')
    ids = [i.id for i in images]
    if len(set(ids)) != len(ids) or 0 in ids:
        sys.exit(f'{path}: image ids must be distinct and not 0 (row 0 is the program)')
    return region, images, allows


# ---------------------------------------------------------------------------------------- ld

def cmd_ld(scenes, out, objdir):
    (align, offset), images, _ = parse(scenes)
    for im in images:
        for o, keep in im.excepts:
            text = subprocess.run([READELF, '-SW', os.path.join(objdir, o + '.o')], capture_output=True,
                                  text=True, check=True).stdout
            fns = re.findall(r'\]\s+\.text\.(\S+)\s+PROGBITS', text)
            missing = [k for k in keep if k not in fns]
            if missing or not fns:
                sys.exit(f'scenes.py ld: {o}.o has no function section for {missing or "any function"}')
            im.functions += [(o, f) for f in fns if f not in keep]
    w = ['/* Made by tools/scenes.py from %s: the scene images, one region after .data.' % os.path.basename(scenes),
         '   Link with -T this -T psyq/psyq.ld (this script first, so its patterns claim their sections). */',
         'SECTIONS {',
         '    __scene_data_end = ADDR(.data) + SIZEOF(.data);']
    w += [
         '    __scene_region_start = __scene_data_end + ((%#x - __scene_data_end) & %#x);' % (offset, align - 1)]
    def body(im, base):
        pats = []
        for o in im.objects + im.code:
            pats.append('        *-%s.o(.text .text.*)' % o)
        for o, f in im.functions:
            pats.append('        *-%s.o(.text.%s)' % (o, f))
        layer_text = [p for p in im.layer if p.startswith('.text')]
        layer_rest = [p for p in im.layer if not p.startswith('.text')]
        if layer_text:
            pats.append('        *libpsyq.a:*(%s)' % ' '.join(layer_text))
        for o in im.objects + im.rodata:
            pats.append('        *-%s.o(.rodata .rodata.*)' % o)
        if layer_rest:
            pats.append('        *libpsyq.a:*(%s)' % ' '.join(layer_rest))
        return (['        __scene_%s_start = .;' % im.name] + pats +
                ['        . = ALIGN(4);',
                 '        __scene_%s_end = .;' % im.name,
                 '        . = %s + ((. - %s + %d) & ~%d);' % (base, base, SECTOR - 1, SECTOR - 1),
                 '    }'])
    region = [im for im in images if not im.boot and not im.heap]
    boot = [im for im in images if im.boot]
    heap = [im for im in images if im.heap]
    pre = [im for im in images if im.preload]
    at = '__scene_region_start'
    for im in region:
        w += ['    __scene_%s_at = %s;' % (im.name, at)]
        w += ['    .scene_%s __scene_%s_at : AT(__scene_%s_at) {' % (im.name, im.name, im.name)] + body(im, '__scene_%s_at' % im.name)
        if ARRANGE == 'resident':
            at = '__scene_%s_at + SIZEOF(.scene_%s)' % (im.name, im.name)
    if ARRANGE == 'resident':
        size = '(%s - __scene_region_start)' % at
    else:
        size = '0'
        for im in region:
            size = 'MAX(%s, SIZEOF(.scene_%s))' % (size, im.name)
    mask = 0
    for im in pre:
        mask |= 1 << im.id
    w += ['    __scene_preloaded = %d;' % mask]
    w += ['    . = __scene_region_start + %s;' % size,
          '    __scene_region_end = .;',
          '}',
          'INSERT AFTER .data;', '']
    if boot:
        im = boot[0]
        # stored after the preloaded image in the EXE (or at the region's start), run at the start of the heap
        lma = ('ADDR(.scene_%s) + SIZEOF(.scene_%s)' % (pre[-1].name, pre[-1].name)) if pre else '__scene_region_start'
        w += ['SECTIONS {',
              '    __scene_boot_vma = ALIGN(ADDR(.bss) + SIZEOF(.bss), 16);',
              '    __scene_boot_lma = %s;' % lma,
              '    .scene_%s __scene_boot_vma : AT(__scene_boot_lma) {' % im.name] + body(im, '__scene_boot_vma')
        end = 'ADDR(.scene_%s) + SIZEOF(.scene_%s)' % (im.name, im.name)
        for h in heap:
            # run right above the boot code, stored at the same distance after its store: one move takes both
            w += ['    __scene_%s_at = ALIGN(%s, 16);' % (h.name, end),
                  '    .scene_%s __scene_%s_at : AT(__scene_boot_lma + (__scene_%s_at - __scene_boot_vma)) {'
                  % (h.name, h.name, h.name)] + body(h, '__scene_%s_at' % h.name)
            end = 'ADDR(.scene_%s) + SIZEOF(.scene_%s)' % (h.name, h.name)
        w += ['    __scene_boot_id = %d;' % im.id,
              '    __scene_boot_size = %s - __scene_boot_vma;' % end,
              '    ASSERT(__scene_boot_lma + __scene_boot_size <= __scene_boot_vma, "the boot image would be moved over itself")',
              '}',
              'INSERT AFTER .bss;', '']
    if CORE:
        # The core's stored copy, last in the EXE (mkisopsx: a Program's row stops before it), run at the window
        # (sdk/psoxide-core.ld's rule; __core_boot copies it there first thing).
        if boot:
            at = '__scene_boot_lma + __scene_boot_size'
        elif pre:
            at = 'ADDR(.scene_%s) + SIZEOF(.scene_%s)' % (pre[-1].name, pre[-1].name)
        else:
            at = '__scene_region_start'
        w += ['SECTIONS {',
              '    __psx_core_store_at = ALIGN(%s, 16);' % at,
              '    .psx_core_store STACK_INIT : AT(__psx_core_store_at) {',
              '        __psx_core_store = .;',
              '        KEEP(*(.psx_core))',
              '        . = ALIGN(4);',
              '        __psx_core_store_end = .;',
              '    }',
              '    __psx_core_store_lma = LOADADDR(.psx_core_store);',
              '    __psx_core_store_load_end = __psx_core_store_lma + SIZEOF(.psx_core_store);',
              '    ASSERT(STACK_INIT == 0x801FDF00, "a program on the core has RAM_SIZE 0x1FE000")',
              '    ASSERT(__psx_core_store_end - __psx_core_store <= 0x2000, "the core image must fit the window")',
              '    ASSERT(__psx_core_store_end > __psx_core_store, "psx-core is not linked with feature link")',
              '}',
              'INSERT AFTER .bss;', '']
    w += ['__scene_overlap = %d;' % (1 if ARRANGE == 'overlap' else 0),
          '__scene_heap = %d;' % sum(1 << im.id for im in heap), '']
    open(out, 'w').write('\n'.join(w))


# -------------------------------------------------------------------------------------- wrap

def cmd_wrap(scenes, out):
    _, images, _ = parse(scenes)
    w = ['# Made by tools/scenes.py: each entry loads its image (PsyqSceneEnter, psyq/src/scenes.rs),',
         '# then tail-calls the real function with the arguments and stack it was given.',
         '    .set noreorder', '    .set noat']
    for im in images:
        for fn in im.entries:
            w += ['    .section .text.__wrap_%s,"ax",@progbits' % fn,
                  '    .globl __wrap_%s' % fn, '    .type __wrap_%s,@function' % fn,
                  '__wrap_%s:' % fn,
                  '    addiu $sp, $sp, -40',
                  '    sw    $ra, 36($sp)', '    sw    $a0, 16($sp)', '    sw    $a1, 20($sp)',
                  '    sw    $a2, 24($sp)', '    sw    $a3, 28($sp)',
                  '    jal   PsyqSceneEnter',
                  '    li    $a0, %d' % im.id,
                  '    lw    $ra, 36($sp)', '    lw    $a0, 16($sp)', '    lw    $a1, 20($sp)',
                  '    lw    $a2, 24($sp)', '    lw    $a3, 28($sp)',
                  '    j     __real_%s' % fn,
                  '    addiu $sp, $sp, 40',
                  '    .size __wrap_%s, .-__wrap_%s' % (fn, fn)]
    open(out, 'w').write('\n'.join(w) + '\n')


# ------------------------------------------------------------------------------------- check

ROW = re.compile(r'^\s*([0-9a-f]+) +([0-9a-f]+) +([0-9a-f]+) +(\d+) (.*)$')
INPUT = re.compile(r'^(.*):\((\S+)\)$')


def obj_key(path):
    """The map's name for an object: lld.sh links copies named N-NAME.o; archives keep member names."""
    m = re.match(r'^(.*)\((.*)\)$', path)
    if m:
        return os.path.basename(m.group(1)) + '(' + m.group(2) + ')'
    return re.sub(r'^\d+-', '', os.path.basename(path))


def read_map(path, symbols=None):
    """[(vma, size, object key, section, output section)] for every input section of the map; with
    `symbols` (a dict), also each symbol's (object key, section)."""
    out, cur, last = [], None, None
    for line in open(path):
        m = ROW.match(line)
        if not m:
            continue
        vma, size, rest = int(m.group(1), 16), int(m.group(3), 16), m.group(5)
        indent = len(rest) - len(rest.lstrip(' '))
        rest = rest.strip()
        if indent == 0:
            cur = rest
        elif indent == 8:
            i = INPUT.match(rest)
            if i and cur:
                last = (obj_key(i.group(1)), i.group(2))
                out.append((vma, size, last[0], last[1], cur))
            else:
                last = None
        elif indent == 16 and symbols is not None and last and ' ' not in rest:
            symbols.setdefault(rest, last)
    return out


def elf_placement(elf):
    """{global symbol: image or BASE} from the linked ELF's own symbol table (its output section)."""
    secs = {}
    for line in subprocess.run([READELF, '-SW', elf], capture_output=True, text=True, check=True).stdout.splitlines():
        m = re.match(r'^\s*\[\s*(\d+)\]\s+(\S+)', line)
        if m:
            secs[int(m.group(1))] = m.group(2)
    out = {}
    for line in subprocess.run([READELF, '-sW', elf], capture_output=True, text=True, check=True).stdout.splitlines():
        m = re.match(r'^\s*\d+:\s+[0-9a-f]+\s+\d+\s+\S+\s+(GLOBAL|WEAK)\s+\S+\s+(\d+)\s+(\S+)', line)
        if m:
            sec = secs.get(int(m.group(2)), '')
            if sec.startswith('.scene_'):
                out[m.group(3)] = sec[len('.scene_'):]
            elif sec in ('.text', '.data', '.bss'):
                out[m.group(3)] = BASE
    return out


def elf_symbols(elf):
    syms = {}
    for line in subprocess.run([NM, elf], capture_output=True, text=True, check=True).stdout.splitlines():
        p = line.split()
        if len(p) == 3:
            syms.setdefault(p[2], int(p[0], 16))
    return syms


def readelf_objects(paths):
    """{object key: (sections {index: name}, symbols [(name, ndx)], relocs {section: [sym index]})}"""
    objs = {}
    for path in paths:
        sec = subprocess.run([READELF, '-SW', path], capture_output=True, text=True, check=True).stdout
        sym = subprocess.run([READELF, '-sW', path], capture_output=True, text=True, check=True).stdout
        rel = subprocess.run([READELF, '-rW', path], capture_output=True, text=True, check=True).stdout
        # split each output per member ("File: lib.a(member.o)") for an archive
        def per_file(text):
            parts, name = {}, os.path.basename(path)
            for line in text.splitlines():
                m = re.match(r'^File: (.*)$', line)
                if m:
                    name = obj_key(m.group(1)) if '(' in m.group(1) else os.path.basename(m.group(1))
                    continue
                parts.setdefault(name, []).append(line)
            return parts
        S, Y, R = per_file(sec), per_file(sym), per_file(rel)
        for key in S:
            sections = {}
            for line in S[key]:
                m = re.match(r'^\s*\[\s*(\d+)\]\s+(\S+)', line)
                if m:
                    sections[int(m.group(1))] = m.group(2)
            symbols = {}
            for line in Y.get(key, []):
                m = re.match(r'^\s*(\d+):\s+([0-9a-f]+)\s+\S+\s+(\S+)\s+(\S+)\s+\S+\s+(\S+)\s*(\S*)', line)
                if m:
                    ndx = int(m.group(5)) if m.group(5).isdigit() else m.group(5)
                    name = m.group(6) or (sections.get(ndx, '') if m.group(3) == 'SECTION' else '')
                    symbols[int(m.group(1))] = (name, ndx, m.group(3), m.group(4))
            relocs, cur = {}, None
            for line in R.get(key, []):
                m = re.match(r"^Relocation section '\.rel(?:a)?(\S+)'", line)
                if m:
                    cur = relocs.setdefault(m.group(1), [])
                    continue
                m = re.match(r'^[0-9a-f]+\s+([0-9a-f]+)\s+(\S+)', line)
                if m and cur is not None:
                    cur.append(int(m.group(1), 16) >> 8)
            objs[key] = (sections, symbols, relocs)
    return objs


def cmd_check(scenes, elf, mapfile, objpaths):
    """Every linked input section is a node, every relocation an edge. For each image Y, the code that
    can run while Y is not loaded is what is reachable from the program's entry and from the other
    images' code without entering Y (a wrapped entry of Y is the door: it loads Y first). None of
    that may refer to Y, except where an `allow` line says the flow guarantees Y is loaded."""
    _, images, allows = parse(scenes)
    entry_image = {fn: im.name for im in images for fn in im.entries}
    allowed = {(fn, im) for fn, im in allows}
    placed = {}
    for vma, size, key, sec, out in read_map(mapfile):
        if out.startswith('.scene_'):
            placed[(key, sec)] = out[len('.scene_'):]
        elif out in ('.text', '.data', '.bss'):
            placed[(key, sec)] = BASE
    syms = elf_symbols(elf)
    gplace = elf_placement(elf)
    ranges = []
    for im in images:
        lo, hi = syms.get('__scene_%s_start' % im.name), syms.get('__scene_%s_end' % im.name)
        if lo is None or hi is None:
            sys.exit(f'scenes.py check: {elf} has no image {im.name}')
        ranges.append((lo, hi, im.name))
    region = (syms['__scene_region_start'], syms['__scene_region_end'])

    objs = readelf_objects(objpaths)
    # Merged constants (.rodata.str*, .rodata.cst*) appear in the map only as `<internal>`: they went to
    # the output section whose pattern matched them, which is the image that lists the object's
    # constants, else the base (when the object is linked at all).
    consts = {o + '.o': im.name for im in images for o in im.objects + im.rodata}
    linked = {k for k, _ in placed}
    for key, (sections, symbols, relocs) in objs.items():
        for sec in sections.values():
            if sec.startswith(('.rodata.str', '.rodata.cst')) and (key, sec) not in placed and key in linked:
                placed[(key, sec)] = consts.get(key, BASE)
    defs = {}           # global symbol -> node that defines it
    for key, (sections, symbols, relocs) in objs.items():
        for name, ndx, typ, bind in symbols.values():
            if bind in ('GLOBAL', 'WEAK') and isinstance(ndx, int) and name and (key, sections.get(ndx)) in placed:
                defs.setdefault(name, (key, sections[ndx]))
    edges = {}          # node -> [(target node, symbol name)]
    errors = []
    for key, (sections, symbols, relocs) in objs.items():
        for src_sec, refs in relocs.items():
            src = (key, src_sec)
            if src not in placed:
                continue        # not linked (--gc-sections), or not loaded (debug)
            out = edges.setdefault(src, [])
            for si in refs:
                if si not in symbols:
                    continue
                name, ndx, typ, bind = symbols[si]
                if isinstance(ndx, int) and ndx in sections:
                    t = (key, sections[ndx])
                    if t in placed:
                        out.append((t, name if typ != 'SECTION' else sections[ndx]))
                elif ndx == 'UND' and name:
                    if name in entry_image:
                        t = ('scenes_wrap.o', '.text.__wrap_' + name)
                        if placed.get(src) not in (BASE, entry_image[name]):
                            errors.append(f'{key}({src_sec}) in {placed[src]} calls the entry {name} of '
                                          f'{entry_image[name]}: it would load over the code that called it')
                    else:
                        t = defs.get(name[len('__real_'):] if name.startswith('__real_') else name)
                    if t is not None:
                        out.append((t, name))
    roots = [defs[n] for n in ('__core_boot', '_start') if n in defs]
    for im in images:
        y = im.name
        seen = set(roots) | {n for n, p in placed.items() if p not in (BASE, y)}
        parent = {}
        todo = list(seen)
        while todo:
            n = todo.pop()
            for t, name in edges.get(n, []):
                if placed.get(t) == y:
                    if n[0] == 'scenes_wrap.o' and name.startswith('__real_'):
                        continue    # the door: the wrapper has loaded the image
                    fn = n[1].split('.text.', 1)[1] if n[1].startswith('.text.') else n[1]
                    stem = n[0][:-2] if n[0].endswith('.o') else n[0]
                    if placed[n] == BASE and ((fn, y) in allowed or (stem, y) in allowed):
                        continue
                    path, p = [], n
                    while p in parent and len(path) < 6:
                        p = parent[p]
                        path.append('%s(%s)' % p)
                    errors.append(f'{n[0]}({n[1]}) in {placed[n]}, which runs while {y} may not be loaded, '
                                  f'refers to {name} in {y}' + (' (reached from ' + ' <- '.join(path) + ')' if path else ''))
                elif t not in seen:
                    seen.add(t)
                    parent[t] = n
                    todo.append(t)
    for im in images:
        for fn in im.entries:
            if fn not in gplace:
                errors.append(f'entry {fn} of {im.name} is not linked')
            elif gplace[fn] != im.name:
                errors.append(f'entry {fn} is not in {im.name} ({gplace[fn]})')
    for e in sorted(set(errors)):
        print('scenes:', e)
    if errors:
        print(f'scenes: {len(set(errors))} reference(s) into an image that may not be loaded')
        sys.exit(1)
    for lo, hi, name in ranges:
        print(f'scenes: {name} {lo:#x}..{hi:#x} {hi - lo} bytes')
    print(f'scenes: region {region[0]:#x}..{region[1]:#x} {region[1] - region[0]} bytes; no reference crosses')


# -------------------------------------------------------------------------------------- pack

def word_fnv(data):
    h = 0x811C9DC5
    for (w,) in struct.iter_unpack('<I', data):
        h = ((h ^ w) * 0x01000193) & 0xFFFFFFFF
    return h


def image_map(mapfile, name, out):
    """The lines of one image's output section, as the map of a headerless blob (what the SDK's
    hazard tools read with --raw: its code is the span of its .text input sections)."""
    lines, inside = [], False
    for line in open(mapfile):
        m = ROW.match(line)
        if m and len(m.group(5)) - len(m.group(5).lstrip(' ')) == 0:
            inside = m.group(5).strip() == '.scene_' + name
            continue
        if inside and m:
            lines.append(line)
    open(out, 'w').write('     VMA      LMA     Size Align Out     In      Symbol\n' + ''.join(lines))


def cmd_cut(scenes, elf, outdir):
    """Each image's bytes (padded to whole sectors) and its map, SCENES.<NAME>.bin and .map in OUTDIR."""
    _, images, _ = parse(scenes)
    syms = elf_symbols(elf)
    region = (syms['__scene_region_start'], syms['__scene_region_end'])
    for im in images:
        base = os.path.join(outdir, 'SCENES.%s' % im.name)
        subprocess.run([OBJCOPY, '-O', 'binary', '-j', '.scene_' + im.name, elf, base + '.bin'], check=True)
        data = open(base + '.bin', 'rb').read()
        data += b'\0' * ((-len(data)) % SECTOR)
        start = syms['__scene_%s_start' % im.name]
        if not im.boot and not im.heap and (start < region[0] or start + len(data) > region[1] or (ARRANGE == 'overlap' and start != region[0])):
            sys.exit(f'scenes.py cut: {im.name} is not inside the region')
        open(base + '.bin', 'wb').write(data)
        image_map(os.path.splitext(elf)[0] + '.map', im.name, base + '.map')


def cmd_patch(exe, mapfile, blob, blobmap, tools):
    """Patch the load-delay hazards of one image's layer code with the SDK's hazard-patch, through the
    resident trampoline array, the way it patches a boot-only overlay: a copy of the program with the
    image stored after its load and a map that declares it as the overlay (`.boot`, `__boot_start`,
    `__boot_load_end`). The program's array and the image come back patched; the scan must find none."""
    import shutil
    import tempfile
    prog = bytearray(open(exe, 'rb').read())
    image = open(blob, 'rb').read()
    rows = [ROW.match(l) for l in open(blobmap)]
    rows = [m for m in rows if m]
    run = int(rows[0].group(1), 16)
    text_lo = struct.unpack_from('<I', prog, 0x18)[0]
    pc, payload = struct.unpack_from('<I', prog, 0x10)[0], struct.unpack_from('<I', prog, 0x1C)[0]
    lma = text_lo + len(prog) - 0x800
    lines, inside, start_at = [], False, None
    CORE_MAP = any(l.rstrip().endswith(' __core_boot') for l in open(mapfile))
    for line in open(mapfile):
        m = ROW.match(line)
        if m and len(m.group(5)) - len(m.group(5).lstrip(' ')) == 0:
            inside = m.group(5).strip().startswith('.scene_')
        if inside:
            continue
        lines.append(line)
        if m and m.group(5).strip() == '_start':
            start_at = int(m.group(1), 16)
            lines.append(line.replace('_start', '__boot_entry'))
    end = run + len(image)
    lines.append('%8x %8x %8x     8 .boot\n' % (run, lma, len(image)))
    lines.append('%8x %8x        0     1         __boot_start = .\n' % (run, run))
    lines += open(blobmap).readlines()[1:]
    lines.append('%8x %8x        0     1         __boot_load_end = .\n' % (end, end))
    tmp = tempfile.mkdtemp()
    try:
        texe, tmap = os.path.join(tmp, 'p.exe'), os.path.join(tmp, 'p.map')
        open(tmap, 'w').write(''.join(lines))
        t = bytearray(prog) + image
        t += b'\0' * ((-len(t)) % SECTOR)
        # the SDK's tools take a core program's entry as `__core_boot` (the header's PC), else `_start`
        struct.pack_into('<II', t, 0x10, pc if CORE_MAP else start_at, 0)
        struct.pack_into('<I', t, 0x1C, len(t) - 0x800)
        open(texe, 'wb').write(t)
        for tool in ('hazard-patch', 'hazard-scan'):
            r = subprocess.run([os.path.join(tools, tool), '--map', tmap, texe])
            if r.returncode:
                sys.exit(f'scenes.py patch: {tool} failed on {blob}')
        t = open(texe, 'rb').read()
        out = bytearray(t[:len(prog)])
        struct.pack_into('<I', out, 0x10, pc)
        struct.pack_into('<I', out, 0x1C, payload)
        open(exe, 'wb').write(out)
        open(blob, 'wb').write(t[len(prog):len(prog) + len(image)])
    finally:
        shutil.rmtree(tmp)


def cmd_splice(scenes, outdir, exe):
    """Put into the EXE what it carries after the program's load: the scene core's stored copy (core
    mode), the preloaded image at its place in the region, the boot code (patched, from cmd_cut and
    cmd_patch) where `__core_boot` or `__boot_entry` moves it from; grow the header's size to cover them."""
    _, images, _ = parse(scenes)
    elf = os.path.splitext(exe)[0] + '.elf'
    syms = elf_symbols(elf)
    prog = bytearray(open(exe, 'rb').read())
    load = struct.unpack_from('<I', prog, 0x18)[0]
    data_end = syms['__data_end']
    pieces = []
    for im in [i for i in images if i.preload] + [i for i in images if i.boot or i.heap]:
        base = os.path.join(outdir, 'SCENES.%s' % im.name)
        run = int(ROW.match(open(base + '.map').readlines()[1]).group(1), 16)
        if im.preload:
            at = run
        else:   # the boot code and the heap images, stored as they run, from the boot code's store
            at = syms['__scene_boot_lma'] + run - syms['__scene_boot_vma']
        pieces.append((im.name, at, open(base + '.bin', 'rb').read()))
    for name, addr, data in pieces:
        if addr < data_end:
            sys.exit(f'scenes.py splice: {name} would be stored inside the program')
        at = 0x800 + addr - load
        if len(prog) < at + len(data):
            prog.extend(b'\0' * (at + len(data) - len(prog)))
        prog[at:at + len(data)] = data
        print(f'scenes: {name} carried in the EXE at {addr:#x} ({len(data)} bytes)')
    prog += b'\0' * ((-len(prog)) % SECTOR)
    struct.pack_into('<I', prog, 0x1C, len(prog) - 0x800)
    open(exe, 'wb').write(prog)


def cmd_manifest(scenes, outdir, out):
    """The mkisopsx --scenes manifest (scene table v2) for the program and its region images: the images
    the EXE carries (`preload`, every one with `arrange resident`) on the program's `carries=`."""
    _, images, _ = parse(scenes)
    region = sorted([im for im in images if not im.boot], key=lambda i: i.id)
    syms = elf_symbols(os.path.join(outdir, [f for f in os.listdir(outdir) if f.endswith('.elf')][0]))
    lo, hi = syms['__scene_region_start'], syms['__scene_region_end']
    hi = lo + ((hi - lo + SECTOR - 1) & ~(SECTOR - 1))
    for im in region:
        if im.heap:     # the reserve reaches up over the heap image
            end = syms['__scene_%s_at' % im.name] + os.path.getsize(os.path.join(outdir, 'SCENES.%s.bin' % im.name))
            hi = max(hi, end)
    ids = [im.id for im in region]
    if ids != list(range(1, len(ids) + 1)):
        sys.exit('scenes.py manifest: the region images must have ids 1..n (their image index)')
    carried = [im.name + '.OVL' for im in region if im.preload]
    text = ['# Made by tools/scenes.py: the scene table v2 for mkisopsx --scenes',
            'program PSX.EXE - reserve=%#x-%#x' % (lo, hi) + (' carries=' + ','.join(carried) if carried else '')]
    starts = {}
    for im in region:
        base = os.path.join(outdir, 'SCENES.%s' % im.name)
        starts[im.name] = int(ROW.match(open(base + '.map').readlines()[1]).group(1), 16)
        # a heap image's memory is the game's heap once another image runs: volatile, read at every load
        cls = 'volatile' if im.heap else 'resident' if ARRANGE == 'resident' else 'swap'
        text.append('overlay %s.OVL SCENES.%s.bin owner=PSX.EXE load=%#x class=%s' %
                    (im.name, im.name, starts[im.name], cls))
    text += ['scene BOOT PSX.EXE start']
    for im in region:
        text.append('scene %s %s.OVL entry=%#x' % (im.name, im.name, starts[im.name]))
    open(out, 'w').write('\n'.join(text) + '\n')


def cmd_embed(img, exe, base_lba):
    """Write SCENES.IMG's rows, with disc LBAs (the file at BASE_LBA), into the program's PSYQ_SCENE_ROWS
    (psyq/src/core_shim.rs), so a switch reads no table."""
    table = open(img, 'rb').read(SECTOR)
    count = struct.unpack_from('<H', table, 0x0A)[0]
    syms = elf_symbols(os.path.splitext(exe)[0] + '.elf')
    prog = bytearray(open(exe, 'rb').read())
    at = 0x800 + syms['PSYQ_SCENE_ROWS'] - struct.unpack_from('<I', prog, 0x18)[0]
    if prog[at:at + 4] != b'PSQT':
        sys.exit('scenes.py embed: no PSYQ_SCENE_ROWS in the EXE')
    rows = bytearray(table[0x40:0x40 + 64 * count])
    for i in range(count):
        kind = rows[i * 64 + 2]
        if kind == 1:
            lba = struct.unpack_from('<I', rows, i * 64 + 4)[0]
            struct.pack_into('<I', rows, i * 64 + 4, lba + base_lba)
    cap = (struct.calcsize('<I') * (2 + 16 * 8))
    if 8 + len(rows) > cap:
        sys.exit('scenes.py embed: more rows than PSYQ_SCENE_ROWS holds')
    prog[at + 4:at + 8] = struct.pack('<I', count)
    prog[at + 8:at + 8 + len(rows)] = rows
    open(exe, 'wb').write(prog)
    print(f'scenes: {count} rows embedded, SCENES.IMG at LBA {base_lba}')


def boot_lma(outdir, exe):
    syms = elf_symbols(os.path.splitext(exe)[0] + '.elf')
    return syms['__scene_boot_lma']


def cmd_pack(scenes, outdir, out, listing):
    """SCENES.IMG from the images cmd_cut wrote (and cmd_patch patched): the table sector, then each."""
    _, images, _ = parse(scenes)
    by_id = {}
    for im in images:
        if im.boot:
            continue    # boot-only code, carried by the EXE: not a scene of the table
        base = os.path.join(outdir, 'SCENES.%s' % im.name)
        data = open(base + '.bin', 'rb').read()
        start = int(ROW.match(open(base + '.map').readlines()[1]).group(1), 16)
        by_id[im.id] = (im, start, data)
    rows, lba = [], 1
    rows.append(struct.pack('<HBBIIIIIIIIHBBHH20s', 0, 0, 0, 0, 0, 0x80010000, 0,
                            0, 0, 0, 0, 0xFFFF, 0, 1, 0, 0, b'PROGRAM'))
    count = max(im.id for im in images) + 1
    lines = []
    for sid in range(1, count):
        if sid not in by_id:
            rows.append(b'\0' * 64)
            continue
        im, start, data = by_id[sid]
        sectors = len(data) // SECTOR
        fnv = word_fnv(data)
        rows.append(struct.pack('<HBBIIIIIIIIHBBHH20s', sid, 1, 0, lba, sectors, start, len(data), 0,
                                fnv, 0, 0, 0xFFFF, 5, 1, 0, 0, im.name.encode()))
        lines.append(f'{sid} {im.name} lba {lba} sectors {sectors} t_addr {start:#x} t_size {len(data)} fnv {fnv:#010x}')
        lba += sectors
    if count > 31:
        sys.exit('scenes.py pack: more rows than one table sector holds')
    body = b''.join(rows)
    header = b'PSXSCN01' + struct.pack('<HHHHHHI', 1, count, 64, 1, 0, 0, word_fnv(body)) + b'\0' * 40
    with open(out, 'wb') as f:
        f.write((header + body).ljust(SECTOR, b'\0'))
        for sid in range(1, count):
            if sid in by_id:
                f.write(by_id[sid][2])
    if listing:
        open(listing, 'w').write('\n'.join(lines) + '\n')
    for line in lines:
        print('scenes:', line)


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    cmd, args = sys.argv[1], sys.argv[2:]
    if cmd == 'ld' and len(args) == 3:
        cmd_ld(*args)
    elif cmd == 'wrap' and len(args) == 2:
        cmd_wrap(*args)
    elif cmd == 'check' and len(args) >= 4:
        cmd_check(args[0], args[1], args[2], args[3:])
    elif cmd == 'flags' and len(args) == 1:
        print(' '.join('--wrap=%s' % fn for im in parse(args[0])[1] for fn in im.entries))
    elif cmd == 'cut' and len(args) == 3:
        cmd_cut(*args)
    elif cmd == 'patch' and len(args) == 5:
        cmd_patch(*args)
    elif cmd == 'embed' and len(args) == 3:
        cmd_embed(args[0], args[1], int(args[2], 0))
    elif cmd == 'manifest' and len(args) == 3:
        cmd_manifest(*args)
    elif cmd == 'splice' and len(args) == 3:
        cmd_splice(*args)
    elif cmd == 'pack' and len(args) in (3, 5):
        cmd_pack(args[0], args[1], args[2], args[4] if len(args) == 5 else None)
    else:
        sys.exit(__doc__)


if __name__ == '__main__':
    main()
