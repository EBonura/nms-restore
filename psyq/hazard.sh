#!/bin/sh
# Post-link, for a program built with every delay-slot filler search on (psyq/build.sh): reroute the
# load-delay hazards the filler leaves with the SDK's hazard-patch, then prove none remain with
# hazard-scan. Both read the ld.lld map of the same link, boot-only overlay included (psyq-boot.ld).
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
"$root/build/psx-gate-target/release/hazard-patch" --map "$2" "$1"
"$root/build/psx-gate-target/release/hazard-scan" --map "$2" "$1"
