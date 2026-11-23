#!/bin/sh
# Compare the layer's card.rs read_dir, find and chain with psx-mc's DirOptions::BIOS forms on
# random directory images (bad checksums, wildcards, broken links, loops). Needs build/psoxide
# (any layer build makes it). Arguments: the number of images (default 300000).
set -e
here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/../.." && pwd)
[ -d "$root/build/psoxide/sdk/crates/psx-mc" ] || { echo "run psyq/build.sh first (it checks out build/psoxide)" >&2; exit 1; }
cd "$here"
cargo run --release --quiet --target-dir "$root/build/psyq-card-target" -- "$@"
