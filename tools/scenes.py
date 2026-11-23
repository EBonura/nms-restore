#!/usr/bin/env python3
"""Scene images of a restored game (R2: the program split into a resident base and images that load
into one shared region, docs/DECISIONS.md "Scene images").

    scenes.py ld      SCENES.TXT OUT.ld OBJDIR              the linker script for the images
    scenes.py wrap    SCENES.TXT OUT.s                      the wrapped entry points (GNU as)
    scenes.py check   SCENES.TXT ELF MAP OBJ... ARCHIVE     fail on a reference into an image that
                                                            may not be loaded
    scenes.py pack    SCENES.TXT ELF OUT.IMG [--list TXT]   cut the images out of the link, write
                                                            SCENES.IMG (table sector, then images)
    scenes.py flags   SCENES.TXT                            the linker's --wrap flags

SCENES.TXT, one directive a line, `#` starts a comment:

    region ALIGN OFFSET        the region starts at the first address after .data that is OFFSET
                               past a multiple of ALIGN (the I-cache knob: the region's code keeps
                               its place in the 4 KB cache whatever the base grows or shrinks to)
    image ID NAME              a new image; ID is its scene id (its row in the table)
    objects NAME...            NMS objects (NAME.o) whose code and constant data go in the image
                               (.text*, .rodata*): their .data and .bss stay in the base, so a
                               reload never resets a variable
    functions NAME FN...       single functions of an object built with -ffunction-sections
                               (.text.FN of NAME.o)
    rodata NAME...             the constant data of objects whose functions are listed one by one
    except NAME FN...          every function of NAME.o (built with -ffunction-sections) but these,
                               which stay in the base, and its constant data
    layer PATTERN...           input sections of the layer (libpsyq.a) by section-name glob, any
                               kind: code, constants, data and bss alike (only BOOT, loaded once,
                               may hold layer data)
    entry FN...                functions the base calls to enter the image: ld --wrap=FN sends
                               every call through __wrap_FN, which loads the image first
    allow FN IMAGE  # reason   the base function FN (or the whole object FN.o) may refer to IMAGE
                               directly (it runs only while IMAGE is loaded, for the reason given)

The check reads each object's relocations (readelf) and the link map: a reference from a section
placed in one image to a section placed in another is an error, a reference from the base into an
image is an error unless it is a wrapped entry, the image's own data kept in the base, or an
`allow` line, and a wrapped entry of one image referred to from inside another image is an error
(it would load over the code that called it).

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
        self.excepts = []


def parse(path):
    region = (0x1000, 0)
    images, allows = [], []
    for n, raw in enumerate(open(path), 1):
        line = raw.split('#', 1)[0].split()
        if not line:
            continue
        kind, args = line[0], line[1:]
        if kind == 'region':
            region = (int(args[0], 0), int(args[1], 0))
        elif kind == 'image':
            images.append(Image(int(args[0]), args[1]))
        elif kind == 'objects':
            images[-1].objects += args
        elif kind == 'functions':
            images[-1].functions += [(args[0], f) for f in args[1:]]
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
         '    __scene_data_end = ADDR(.data) + SIZEOF(.data);',
         '    __scene_region_start = __scene_data_end + ((%#x - __scene_data_end) & %#x);' % (offset, align - 1)]
    for im in images:
        pats = []
        for o in im.objects:
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
        w += ['    .scene_%s __scene_region_start : AT(__scene_region_start) {' % im.name,
              '        __scene_%s_start = .;' % im.name]
        w += pats
        w += ['        . = ALIGN(4);',
              '        __scene_%s_end = .;' % im.name,
              '        . = __scene_region_start + ((. - __scene_region_start + %d) & ~%d);' % (SECTOR - 1, SECTOR - 1),
              '    }']
    size = '0'
    for im in images:
        size = 'MAX(%s, SIZEOF(.scene_%s))' % (size, im.name)
    w += ['    . = __scene_region_start + %s;' % size,
          '    __scene_region_end = .;',
          '}',
          'INSERT AFTER .data;', '']
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
            symbols = []
            for line in Y.get(key, []):
                m = re.match(r'^\s*(\d+):\s+([0-9a-f]+)\s+\d+\s+(\S+)\s+(\S+)\s+\S+\s+(\S+)\s*(\S*)', line)
                if m:
                    ndx = int(m.group(5)) if m.group(5).isdigit() else m.group(5)
                    name = m.group(6) or (sections.get(ndx, '') if m.group(3) == 'SECTION' else '')
                    symbols.append((name, ndx, m.group(3), m.group(4)))
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
    _, images, allows = parse(scenes)
    owner = {}
    for im in images:
        for o in im.objects + im.rodata:
            owner[o + '.o'] = im.name
    entry_image = {fn: im.name for im in images for fn in im.entries}
    allowed = {(fn, im) for fn, im in allows}
    placed = {}         # (object key, section) -> output image
    ranges = []         # (lo, hi, image)
    where = {}          # symbol -> (object key, section)
    for vma, size, key, sec, out in read_map(mapfile, where):
        if out.startswith('.scene_'):
            placed[(key, sec)] = out[len('.scene_'):]
        elif out in ('.text', '.data', '.bss'):
            placed[(key, sec)] = BASE
    syms = elf_symbols(elf)
    gplace = elf_placement(elf)
    for im in images:
        lo, hi = syms.get('__scene_%s_start' % im.name), syms.get('__scene_%s_end' % im.name)
        if lo is None or hi is None:
            sys.exit(f'scenes.py check: {elf} has no image {im.name}')
        ranges.append((lo, hi, im.name))
    region = (syms['__scene_region_start'], syms['__scene_region_end'])

    def at(addr):
        for lo, hi, name in ranges:
            if lo <= addr < hi:
                return name
        if region[0] <= addr < region[1]:
            return '?'      # padding of the region
        return BASE

    # which global symbol a function-level section belongs to, for the allow lines and messages
    def section_function(sec):
        return sec.split('.text.', 1)[1] if sec.startswith('.text.') else sec

    objs = readelf_objects(objpaths)
    errors, seen_entries = [], set()
    for key, (sections, symbols, relocs) in objs.items():
        for src_sec, refs in relocs.items():
            src_img = placed.get((key, src_sec))
            if src_img is None:
                continue        # not linked (--gc-sections)
            src_owner = owner.get(key, src_img)
            for si in refs:
                if si >= len(symbols):
                    continue
                name, ndx, typ, bind = symbols[si]
                if isinstance(ndx, int) and ndx in sections:
                    target_sec = sections[ndx]
                    tgt_img = placed.get((key, target_sec))
                    if tgt_img is None:
                        continue
                    what = '%s(%s)' % (key, target_sec) if typ == 'SECTION' else name
                elif ndx == 'UND' and name:
                    if name in entry_image:
                        tgt = entry_image[name]
                        seen_entries.add(name)
                        if src_img not in (BASE, tgt):
                            errors.append(f'{key}({src_sec}) in {src_img} calls the entry {name} of {tgt}: '
                                          'it would load over the code that called it')
                        continue
                    real = name[len('__real_'):] if name.startswith('__real_') else name
                    if real not in gplace or real.startswith('__scene_'):
                        continue
                    tgt_img = gplace[real]
                    what = real
                    if name.startswith('__real_') and key == 'scenes_wrap.o':
                        continue
                else:
                    continue
                if tgt_img == BASE or tgt_img == src_img:
                    continue
                if src_img == BASE and src_owner == tgt_img:
                    continue        # the image's own data, kept in the base
                fn = section_function(src_sec)
                stem = key[:-2] if key.endswith('.o') else key
                if src_img == BASE and ((fn, tgt_img) in allowed or (stem, tgt_img) in allowed):
                    continue
                errors.append(f'{key}({src_sec}) in {src_img} refers to {what} in {tgt_img}')
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


def cmd_pack(scenes, elf, out, listing):
    _, images, _ = parse(scenes)
    syms = elf_symbols(elf)
    region = (syms['__scene_region_start'], syms['__scene_region_end'])
    blobs = []
    for im in images:
        tmp = out + '.%s.tmp' % im.name
        subprocess.run([OBJCOPY, '-O', 'binary', '-j', '.scene_' + im.name, elf, tmp], check=True)
        data = open(tmp, 'rb').read()
        os.remove(tmp)
        data += b'\0' * ((-len(data)) % SECTOR)
        start = syms['__scene_%s_start' % im.name]
        if start != region[0] or start + len(data) > region[1]:
            sys.exit(f'scenes.py pack: {im.name} is not inside the region')
        blobs.append((im, start, data))
    rows, lba = [], 1
    rows.append(struct.pack('<HBBIIIIIIIIHBBHH20s', 0, 0, 0, 0, 0, 0x80010000, 0,
                            0, 0, 0, 0, 0xFFFF, 0, 1, 0, 0, b'PROGRAM'))
    count = max(im.id for im in images) + 1
    by_id = {im.id: (im, start, data) for im, start, data in blobs}
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
    table = (header + body).ljust(SECTOR, b'\0')
    with open(out, 'wb') as f:
        f.write(table)
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
    elif cmd == 'pack' and len(args) in (3, 5):
        cmd_pack(args[0], args[1], args[2], args[4] if len(args) == 5 else None)
    else:
        sys.exit(__doc__)


if __name__ == '__main__':
    main()
