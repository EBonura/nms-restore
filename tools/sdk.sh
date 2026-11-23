# Sourced by the build scripts: sets $psoxide to a full PSoXide SDK checkout at the
# commit psyq/build.sh pins. The disc tools (mkisopsx, psx-str-cook, psoxide-hazard)
# are built from it.
#
# PSOXIDE_REPO names an existing checkout to use as it is. Without it, the SDK is
# cloned once into build/psoxide-full and checked out at the pin.
# Needs $root (the repository root). Sets $offline to cargo's --offline for an
# existing checkout (its dependencies are already fetched), empty for a fresh clone.
if [ -n "$PSOXIDE_REPO" ]; then
	psoxide=$PSOXIDE_REPO
	offline=--offline
else
	offline=
	psoxide=$root/build/psoxide-full
	pin=$(sed -n 's/^rev=\${PSOXIDE_REV:-\([0-9a-f]*\)}.*/\1/p' "$root/psyq/build.sh")
	pin=${PSOXIDE_REV:-$pin}
	[ -n "$pin" ] || { echo "tools/sdk.sh: no SDK pin found in psyq/build.sh" >&2; exit 1; }
	if [ ! -d "$psoxide/.git" ]; then
		git clone -q --filter=blob:none --no-checkout "${PSOXIDE_SRC:-https://github.com/EBonura/PSoXide}" "$psoxide"
	fi
	# (a fresh --no-checkout clone may already have HEAD at the pin, with no files yet)
	if [ "$(git -C "$psoxide" rev-parse HEAD 2>/dev/null)" != "$pin" ] || [ ! -f "$psoxide/Cargo.toml" ]; then
		git -C "$psoxide" checkout -q -f "$pin"
	fi
fi
