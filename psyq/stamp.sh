#!/bin/sh
# Stamp the architecture gate's receipt into a linked PS-EXE (sdk/docs/GATE.md, G3):
#   psyq/stamp.sh build/360x-cd/360x.exe
# It runs `psx-gate` over the layer (the program both games link), as `psoxide-pgo apply` does for a
# Rust program, and on a pass writes the receipt `mkisopsx` checks. PSYQ_DEBUG=1 builds get a
# diagnostic receipt (the layer's `debug-console` feature), which mkisopsx takes only with --diag.
# Run it after the executable is final (padded to whole sectors): any later change breaks the receipt.
set -e
here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/.." && pwd)
sdk=$root/build/psoxide
# the tool is built from the SDK checkout the disc recipes build mkisopsx from (tools/sdk.sh), so the
# receipt it writes is the format that mkisopsx reads; --sdk is the pinned tree the rules come from
. "$root/tools/sdk.sh"
cargo build -q --release --locked $offline --manifest-path "$psoxide/tools/psoxide-hazard/Cargo.toml" \
	--target-dir "$root/build/psx-gate-target" --bin psx-gate
cd "$root"
"$root/build/psx-gate-target/release/psx-gate" --sdk "$sdk" --crate psyq --program-file psx-gate.toml \
	--receipt "$1" -- build --release ${PSYQ_DEBUG:+--features debug-console}
