#!/bin/sh
# Post-link, for a program built with every delay-slot filler search on (psyq/build.sh): reroute the
# load-delay hazards the filler leaves with the SDK's hazard-patch, then prove none remain with
# hazard-scan. Both read the ld.lld map of the same link. Scene images (tools/scenes.py) are not in the
# EXE: the scene tool scans them on their own.
#
#   psyq/hazard.sh EXE MAP
#   psyq/hazard.sh --image EXE MAP BIN BINMAP   after EXE's own run: a scene image (tools/scenes.py cut
#                                        writes BIN and BINMAP), patched through EXE's trampolines
#
# The tools are built from the SDK checkout the disc recipes build mkisopsx from (tools/sdk.sh), as
# stamp.sh builds psx-gate; the checkout must have the boot-overlay form of hazard-patch.
set -e
here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/.." && pwd)
. "$root/tools/sdk.sh"
cargo build -q --release --locked $offline --manifest-path "$psoxide/tools/psoxide-hazard/Cargo.toml" \
	--target-dir "$root/build/psx-gate-target" --bin hazard-patch --bin hazard-scan
if [ "$1" = --image ]; then
	python3 -I "$root/tools/scenes.py" patch "$2" "$3" "$4" "$5" "$root/build/psx-gate-target/release"
	exit
fi
# The program starts at the scene core's `__core_boot`, which the SDK's tools (at this pin) know as the entry.
"$root/build/psx-gate-target/release/hazard-patch" --map "$2" "$1"
"$root/build/psx-gate-target/release/hazard-scan" --map "$2" "$1"
