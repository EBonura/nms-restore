#!/bin/sh
# Link a layer program with the SDK's linker: rust-lld, from the toolchain psyq/ pins, as every
# psoxide.ld program links. It takes the arguments mipsel-none-elf-ld took. Its map is ld.lld's,
# the form the SDK's hazard-patch reads. GNU as's and gcc's objects are first rewritten by
# llvm-objcopy (lld wants locals before globals in the symbol table) into a temporary folder;
# archives are passed as they are. The boot-only overlay's LMA overlaps `.bss` by design
# (psyq-boot.ld), so the section-overlap check is off.
set -e
here=$(cd "$(dirname "$0")" && pwd)
bin=$(cd "$here" && echo "$(rustc --print sysroot)/lib/rustlib/$(rustc -vV | sed -n 's/^host: //p')/bin")
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
n=$#
while [ $n -gt 0 ]; do
	a=$1; shift; n=$((n - 1))
	case $a in
	*.o) copy=$tmp/$n-$(basename "$a"); "$bin/llvm-objcopy" "$a" "$copy"; set -- "$@" "$copy" ;;
	*) set -- "$@" "$a" ;;
	esac
done
exec "$bin/rust-lld" -flavor gnu -m elf32ltsmip --no-check-sections "$@"
