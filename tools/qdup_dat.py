#!/usr/bin/env python3
"""Make QD Up's QDUP.DAT: the one file its CD loader reads, as 360X's 360X.DAT.

    qdup_dat.py DATA_ROOT OUT.DAT [--list OUT.txt] FILE...

DATA_ROOT is the 1996 data folder (POOL2 of P_200996.ZIP); each FILE is a path
under it ("GFX/FRONT.TGA", "TABLES/CRYPT/BRIGHT.TAB").

The disc's root directory is one ISO sector, which holds about forty names;
QD Up's files (the option screen, the models and, now, three tables with
their palettes) are more. 360X puts its data in one file with an index
(tools/x360_dat.py, FILES.C FileInfo), and QD Up's loader (FILE_IO.C LoadFile,
disc build) does the same. The format is 360X's, with names that carry the
folder:

  index    sectors 0 and 1: a LONG count, then 32-byte entries from byte 4:
             +0   name, NUL-terminated, at most 23 characters: the name of the
                  file's last folder, a backslash and the file's name, upper
                  case ("POTTERS\\POTTERUS.ESH"); a file in the data folder
                  itself has its name alone ("COSINE.TAB")
             +24  LONG first sector, counted from the DAT's first sector
             +28  LONG size in bytes
           sorted by byte value; two sectors hold 127 entries
  files    each from a sector of its own, from sector 2, in the order given (the
           order the game opens them, so each read starts near where the last
           one ended, qdup/disc.sh);
           the size is the file's length, and each file owns
           (size >> 11) + 1 sectors, the number a loader that reads that many
           reads: the next file starts where a read ends. A zero sector closes
           the DAT.

The folder is part of the name because the Crypt's BRIGHT.TAB, GRRR.PAL and
POOLPAL.TAB share their names with the Potters folder's (different files).
"""
import argparse
import os
import struct
import sys

SECTOR = 2048
INDEX_SECTORS = 2
ENTRY = 32
MAX_ENTRIES = (INDEX_SECTORS * SECTOR - 4) // ENTRY


def key(path):
    parts = path.replace('\\', '/').split('/')
    base = parts[-1].upper()
    return (parts[-2].upper() + '\\' + base) if len(parts) > 1 and parts[-2] else base


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('data')
    ap.add_argument('out')
    ap.add_argument('--list')
    ap.add_argument('files', nargs='+')
    a = ap.parse_args()
    ents = []
    for f in a.files:
        k = key(f)
        if len(k) > 23:
            sys.exit(f'name too long: {k}')
        ents.append((k, open(os.path.join(a.data, f), 'rb').read()))
    names = [k for k, _ in ents]
    if len(set(names)) != len(names):
        sys.exit('duplicate names: ' + ', '.join(sorted(n for n in names if names.count(n) > 1)))
    if len(ents) > MAX_ENTRIES:
        sys.exit('too many files')
    # the files lie in the order given (the order the game opens them); only the index is sorted
    index = bytearray(INDEX_SECTORS * SECTOR)
    struct.pack_into('<i', index, 0, len(ents))
    body = bytearray()
    sector = INDEX_SECTORS
    lines = []
    where = {}
    for k, data in ents:
        where[k] = (sector, len(data))
        lines.append(f'{k} sector {sector} size {len(data)}')
        n = (len(data) >> 11) + 1  # the sectors the loader reads: one more than whole sectors hold
        body += data + bytes(n * SECTOR - len(data))
        sector += n
    for i, k in enumerate(sorted(names, key=lambda n: n.encode())):
        o = 4 + i * ENTRY
        index[o:o + len(k)] = k.encode()
        struct.pack_into('<ii', index, o + 24, *where[k])
    body += bytes(SECTOR)
    open(a.out, 'wb').write(bytes(index) + bytes(body))
    if a.list:
        open(a.list, 'w').write('\n'.join(lines) + '\n')
    print(f'{a.out}: {len(ents)} files, {sector + 1} sectors')


if __name__ == '__main__':
    main()
