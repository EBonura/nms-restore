#!/usr/bin/env python3
"""Make 360X's 360X.DAT: the one big file NMS's CD loader reads.

    x360_dat.py DATA_ROOT OUT.DAT [--list OUT.txt] [--extra DIR] [--order LOADORDER.TXT]

DATA_ROOT is the 1996 tree (the unpacked 360X.ZIP); the files of its INTRO,
LEVEL1, MAPS, MISC and SOUND folders go in, the folders the game loads from
(LEVEL2 to LEVEL9 are empty in the 1996 tree). --extra adds the files of
DIR's LEVEL1 to LEVEL9 and SATURN folders after them: the missions' creature
files tools/x360_txt2spr.py --stage builds from NMS's GRAPHICS/LEVELn art, and
the Saturn sprites tools/x360_scs2spr.py --stage converts.

The format is the one 360X's FILES.C reads with cdmaster=YES:

  InitCD      CdSearchFile("\\360X.DAT;1"), then CdRead of 8 sectors from
              the file's first sector into indexbuffer[16384]: the index.
  FileInfo    index: a LONG count, then 32-byte entries from byte 4:
                +0   file name, NUL-terminated (no folder: FileInfo drops
                     everything up to the last '/' of the name the game asks
                     for), at most 15 characters
                +16  LONG first sector, counted from the DAT's first sector
                +20  LONG size in bytes
                +24  unused (zero)
              FileInfo binary-searches the entries with strcmp, so they are
              sorted by byte value; base names must be unique. 16384 bytes
              hold 511 entries.
  LoadFile    CdControlB(CdlSeekL) to DAT sector + first sector, then CdRead
              of (size >> 11) + 1 sectors into a buffer of that many
              sectors (MyMalloc).

Here the index takes sectors 0 to 7 and each file starts on a sector of its
own from sector 8, in the order of --order's list (one base name a line, '#'
starts a comment; the files the list does not name follow, in folder order) or,
with no list, in folder order. The game asks for its files one after another in
the same order every time, and the drive needs far less time to start a read
that begins close to where the last one ended (360x/LOADORDER.TXT). The size is the file's length in bytes,
and LoadFile reads (size >> 11) + 1 sectors, so each file owns exactly that many
sectors (one more than it needs when its length is a multiple of 2048) and the
next file starts on the sector the read ends at. One zero sector closes the DAT
(the layer reads one sector past every read, psyq/src/cd.rs).

A CD build from 2007 of the same code puts only the index in 360X.DAT and
points its entries at the files in the disc's folders, with sizes rounded up
to whole sectors; FILES.C reads both the same way ("STARTING POINT FOR BIG
.DAT FILE", it calls startdatsector). One file keeps the disc to the root
directory and makes the layout independent of the ISO tool.

Every name is looked up afterwards with a port of FileInfo's search, and the
entry checked against the file.
"""
import argparse
import os
import struct
import sys

FOLDERS = ['SOUND', 'INTRO', 'LEVEL1', 'MAPS', 'MISC']
SECTOR = 2048
INDEX_SECTORS = 8
ENTRY = 32
MAX_ENTRIES = (INDEX_SECTORS * SECTOR - 4) // ENTRY


def file_info(index, name):
    """FILES.C FileInfo, as written: (first sector, size) or None."""
    noof = struct.unpack_from('<i', index, 0)[0]
    cp = name.rsplit('/', 1)[-1].encode()
    lo, hi = 0, noof - 1
    while True:
        pos = (lo + hi) >> 1
        ep = index[4 + pos * ENTRY:4 + pos * ENTRY + 16].split(b'\0')[0]
        ret = (ep > cp) - (ep < cp)
        if ret == 0:
            break
        if lo == hi:
            return None
        if ret < 0:
            lo = pos if lo != pos else pos + 1
        else:
            hi = pos
    return struct.unpack_from('<ii', index, 4 + pos * ENTRY + 16)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('data')
    ap.add_argument('out')
    ap.add_argument('--list')
    ap.add_argument('--extra')
    ap.add_argument('--order')
    a = ap.parse_args()

    files = []
    for d in FOLDERS:
        for n in sorted(os.listdir(os.path.join(a.data, d))):
            p = os.path.join(a.data, d, n)
            if os.path.isfile(p):
                files.append((d, n, p))
    if a.extra:
        for d in sorted(os.listdir(a.extra)):
            if not d.startswith(('LEVEL', 'SATURN')):
                continue
            for n in sorted(os.listdir(os.path.join(a.extra, d))):
                files.append((d, n, os.path.join(a.extra, d, n)))
    names = [n for _, n, _ in files]
    if a.order:
        rank = {}
        for line in open(a.order).read().splitlines():
            line = line.split('#')[0].strip()
            if line:
                rank.setdefault(line, len(rank))
        unknown = sorted(set(rank) - set(names))
        if unknown:
            sys.exit(f'{a.order} names files that are not on the disc: {unknown}')
        # sorted() is stable: unlisted files keep their folder order, after the listed ones
        files.sort(key=lambda f: rank.get(f[1], len(rank)))
    dup = sorted({n for n in names if names.count(n) > 1})
    if dup:
        sys.exit(f'base names in more than one folder: {dup}')
    if len(files) > MAX_ENTRIES:
        sys.exit(f'{len(files)} files, the index holds {MAX_ENTRIES}')
    if any(len(n.encode()) > 15 for n in names):
        sys.exit('a name is longer than 15 characters')

    sector = INDEX_SECTORS
    body = bytearray()
    where = {}
    for d, n, p in files:
        data = open(p, 'rb').read()
        where[n] = (sector, len(data), d)
        used = (len(data) >> 11) + 1	# the sectors LoadFile reads: one more than whole sectors hold
        body += data + bytes(used * SECTOR - len(data))
        sector += used
    body += bytes(SECTOR)

    index = bytearray(INDEX_SECTORS * SECTOR)
    struct.pack_into('<i', index, 0, len(files))
    for i, n in enumerate(sorted(names, key=lambda s: s.encode())):
        first, size, _ = where[n]
        struct.pack_into('<16sii8x', index, 4 + i * ENTRY, n.encode(), first, size)

    for d, n, p in files:
        got = file_info(index, f'../{d}/{n}')
        if got != where[n][:2]:
            sys.exit(f'FileInfo finds {n} as {got}, wrote {where[n][:2]}')
        first, size = got
        off = (first - INDEX_SECTORS) * SECTOR
        if body[off:off + size] != open(p, 'rb').read():
            sys.exit(f'{n}: data mismatch')

    with open(a.out, 'wb') as f:
        f.write(index + body)
    total = len(index) + len(body)
    print(f'{a.out}: {len(files)} files, {total} bytes ({total // SECTOR} sectors)')
    if a.list:
        with open(a.list, 'w') as f:
            f.write('sector size folder/name\n')
            for d, n, p in files:
                f.write(f'{where[n][0]:6d} {where[n][1]:8d} {d}/{n}\n')


if __name__ == '__main__':
    main()
