#!/bin/sh
# Post-link, for a program built with every delay-slot filler search on (psyq/build.sh): reroute the
# load-delay hazards the filler leaves with the SDK's hazard-patch, then prove none remain with
# hazard-scan. Both read the ld.lld map of the same link. Scene images (tools/scenes.py) are not in the
# EXE: the scene tool scans them on their own.
#
#   psyq/hazard.sh EXE MAP
#
# The tools are built from the SDK checkout the disc recipes build mkisopsx from (tools/sdk.sh), as
# stamp.sh builds psx-gate; the checkout must have the boot-overlay form of hazard-patch.
set -e
here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/.." && pwd)
. "$root/tools/sdk.sh"
cargo build -q --release --locked $offline --manifest-path "$psoxide/tools/psoxide-hazard/Cargo.toml" \
	--target-dir "$root/build/psx-gate-target" --bin hazard-patch --bin hazard-scan
# RESTORE: PLATFORM. The program starts at `__core_boot` (src/boot.rs, the scene core's entry ahead of
# psx-rt's `_start`), which the tools do not know as an entry yet: they check that the header's initial PC
# is the map's `_start`. So they run with the PC set to `_start`, and the real one goes back after.
pc() {	# pc EXE [NEW]: print the header's initial PC, or set it to NEW (hex)
	python3 -I -c 'import struct,sys
f=open(sys.argv[1],"r+b"); f.seek(16)
if len(sys.argv)>2: f.write(struct.pack("<I",int(sys.argv[2],16)))
else: print("%08x" % struct.unpack("<I",f.read(4))[0])' "$@"
}
entry=$(pc "$1")
start=$(awk '$NF=="_start" && $1 ~ /^[0-9a-f]+$/ {print $1; exit}' "$2")
[ -n "$start" ] || { echo "hazard.sh: no _start in $2"; exit 1; }
pc "$1" "$start"
"$root/build/psx-gate-target/release/hazard-patch" --map "$2" "$1" || { pc "$1" "$entry"; exit 1; }
"$root/build/psx-gate-target/release/hazard-scan" --map "$2" "$1" || { pc "$1" "$entry"; exit 1; }
pc "$1" "$entry"
