#!/bin/sh
# Build the Psy-Q compatibility layer into build/psyq/libpsyq.a.
#
# The PSoXide SDK comes from a clean sparse clone at build/psoxide, checked out
# at PSOXIDE_REV (origin/main of https://github.com/EBonura/PSoXide). PSOXIDE_SRC names
# the repository to clone from.
#
# RESTORE: PLATFORM. The pad is read with `psx_pad::Controllers::digital()`, a poll with no
# handshake (src/kernel.rs); the poll also accepts the latched /ACK that PCSX-ReARMed uses.
# docs/DECISIONS.md, Both games. The pin's history is in this file's git log.
set -e
here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/.." && pwd)
rev=${PSOXIDE_REV:-6bfe478ff0e0c6e226cdf67b164df311c1658fec}
src=${PSOXIDE_SRC:-https://github.com/EBonura/PSoXide}
sdk=$root/build/psoxide

if [ ! -d "$sdk/.git" ]; then
	git clone -q --no-checkout "$src" "$sdk"
	git -C "$sdk" sparse-checkout init --cone
	git -C "$sdk" sparse-checkout set sdk crates/psx-hw crates/psx-gate tools
fi
# (a clone made before the policy file was read from tools/ lacks it)
git -C "$sdk" sparse-checkout set sdk crates/psx-hw crates/psx-gate tools
# (a clone whose HEAD is already $rev still needs its files checked out)
if [ "$(git -C "$sdk" rev-parse HEAD 2>/dev/null)" != "$rev" ] || [ ! -d "$sdk/sdk" ]; then
	git -C "$sdk" checkout -q -f "$rev"
fi

# LLVM's delay-slot filler may move a load into a branch delay slot whose
# target reads the register at once, inside the R3000's load delay. The layer is built with the
# SDK's policy, every filler search on (PSX_DELAY_SLOT_FLAGS in the SDK's tools/sdk-examples.mk:
# the two searches the compiler leaves off by default), and 360x/build.sh runs the SDK's post-link
# hazard-patch on the program, which reroutes each hazard the filler leaves.
cd "$here"
# The two flag lists are read from the SDK's tools/sdk-examples.mk (PSX_DELAY_SLOT_FLAGS and
# PSX_PANIC_ABORT_FLAGS), the way the SDK's own programs get them, so a change of policy there
# reaches the layer with the next pin. `-Zfmt-debug=none` is the layer's own addition.
policy() { sed -n "s/^$1 := //p" "$sdk/tools/sdk-examples.mk" | tr -d '"' | tr ',' ' '; }
delay_slot=$(policy PSX_DELAY_SLOT_FLAGS)
panic_abort=$(policy PSX_PANIC_ABORT_FLAGS)
[ -n "$delay_slot" ] && [ -n "$panic_abort" ] || { echo "psyq/build.sh: no flag policy in $sdk/tools/sdk-examples.mk" >&2; exit 1; }
# psyq.ld is a fork of the SDK's sdk/psoxide.ld (GNU ld and lld, RAM size, the boot overlay and the
# I-cache placement are the layer's). Its PS-X EXE header block is the SDK's text: stop when the SDK's
# header changes, so the layer's copy is never left behind silently.
header_words() { grep -o 'LONG([^)]*)[^;]*;' "$1" | sed -e 's/ *\/\*.*//' -e 's/DEFINED(__initial_pc) ? __initial_pc : _start/_start/'; }
[ "$(header_words "$sdk/sdk/psoxide.ld")" = "$(header_words "$here/psyq.ld")" ] \
	|| { echo "psyq/build.sh: the PS-X EXE header block of psyq.ld differs from $sdk/sdk/psoxide.ld: port the SDK's change" >&2; exit 1; }
# RESTORE: PLATFORM (owner's decision, 2026-11-13: "gate the debug printout"). The debug console
# is off in the layer unless PSYQ_DEBUG is set (the headless tools that read it, tests/ and the
# film check, build with PSYQ_DEBUG=1); the disc builds leave it off.
# A test build with the CD fault hook (PSYQ_TEST_CD_FAULT=1, cd.rs CdTestFault) gets its own target and library,
# build/psyq-tcd, so the shipped builds' library is never one with the hook.
lib=psyq${PSYQ_TEST_CD_FAULT:+-tcd}
# RESTORE: PLATFORM (owner's request, 2026-11-22). The boot splash (src/splash.rs) draws the SDK's
# Bonnie Studios mark, cooked from the pinned SDK's psx-splash source into build/bonniemark.bin, beside
# the PSX Archive logo, packed from art/psxarchive-logo.png into build/psxarchive.bin; the layer includes both.
mkdir -p "$root/build"
python3 -I "$root/tools/bonnie_mark.py" "$sdk/sdk/crates/psx-splash/src/logo.rs" "$root/build/bonniemark.bin.tmp"
cmp -s "$root/build/bonniemark.bin.tmp" "$root/build/bonniemark.bin" 2>/dev/null && rm "$root/build/bonniemark.bin.tmp" \
	|| mv "$root/build/bonniemark.bin.tmp" "$root/build/bonniemark.bin"
python3 -I "$root/tools/psxarchive_logo.py" "$here/art/psxarchive-logo.png" "$root/build/psxarchive.bin.tmp"
cmp -s "$root/build/psxarchive.bin.tmp" "$root/build/psxarchive.bin" 2>/dev/null && rm "$root/build/psxarchive.bin.tmp" \
	|| mv "$root/build/psxarchive.bin.tmp" "$root/build/psxarchive.bin"
RUSTFLAGS="$panic_abort -Zfmt-debug=none $delay_slot" \
cargo build --release ${PSYQ_DEBUG:+--features debug-console} ${PSYQ_TEST_CD_FAULT:+--features test-cd-fault} --target mipsel-sony-psx \
	-Zbuild-std=core -Zbuild-std-features=compiler-builtins-mem \
	--target-dir "$root/build/$lib/target"
mkdir -p "$root/build/$lib"
cp "$root/build/$lib/target/mipsel-sony-psx/release/libpsyq.a" "$root/build/$lib/libpsyq.a"
