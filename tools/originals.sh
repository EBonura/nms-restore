#!/bin/sh
# Unpack NMS's original archives into build/originals, where the build scripts look for
# the game data by default.
#
#   tools/originals.sh DIR
#
# DIR holds the three archives (the names they have in PSX Archive's set):
#   360X.ZIP          360X for the PS1, July 1996: source, data, 1996 builds
#   20SEPT96.ZIP      360X for the Saturn, 19 Sep 1996 (the 360X disc takes some sprites from it)
#   P_200996.ZIP      QD Up, 19 Sep 1996
#
# Each archive is checked against its sha256 before it is unpacked. P_200996.ZIP has five
# damaged files, each one flipped bit from a copy made in September 1996; the script fixes the
# five bytes in a copy and checks the result's sha256 before unpacking it. A copy that is
# already repaired (P_200996_REPAIRED.ZIP) is used as it is.
#
# Output:
#   build/originals/360x          360X.ZIP          (X360_DATA)
#   build/originals/360x-saturn   20SEPT96.ZIP      (X360_SATURN)
#   build/originals/qdup          P_200996.ZIP      (QDUP_DATA is its WORK/SONY/POOL2)
set -e
[ $# -eq 1 ] || { echo "usage: tools/originals.sh DIR" >&2; exit 2; }
src=$1
root=$(cd "$(dirname "$0")/.." && pwd)
out=$root/build/originals
mkdir -p "$out"

sha() { shasum -a 256 "$1" | cut -d' ' -f1; }
check() {
	[ -f "$1" ] || { echo "missing $1" >&2; exit 1; }
	[ "$(sha "$1")" = "$2" ] || { echo "$1: sha256 is not the expected $2" >&2; exit 1; }
}
unpack() {
	rm -rf "$out/$2.tmp"
	mkdir "$out/$2.tmp"
	unzip -q "$1" -d "$out/$2.tmp"
	rm -rf "$out/$2"
	mv "$out/$2.tmp" "$out/$2"
	echo "unpacked $(basename "$1") into build/originals/$2"
}

check "$src/360X.ZIP" 3e8f0213b008e1782e9997130de65eaadc514358c5e8330a7e39720c18c1abce
unpack "$src/360X.ZIP" 360x
check "$src/20SEPT96.ZIP" 3c101a60b749064d8055d1dd0087ac71f57038d6193ebce2b44f6da861beb74a
unpack "$src/20SEPT96.ZIP" 360x-saturn

repaired=9f20a391d69d6a8a8133151d30c065302c1036b7724ad4b38413f55fd6fb22ba
if [ -f "$src/P_200996_REPAIRED.ZIP" ]; then
	check "$src/P_200996_REPAIRED.ZIP" $repaired
	unpack "$src/P_200996_REPAIRED.ZIP" qdup
else
	check "$src/P_200996.ZIP" 00d2a31920ebf53775cc4eb681799e1044ab9f726401e5963f1e123ad96b79c8
	rm -f "$out/P_200996_REPAIRED.ZIP"
	cp "$src/P_200996.ZIP" "$out/P_200996_REPAIRED.ZIP"
	chmod u+w "$out/P_200996_REPAIRED.ZIP"
	# offset in the zip, repaired byte (octal), file
	while read -r offset byte file; do
		printf "\\$byte" | dd of="$out/P_200996_REPAIRED.ZIP" bs=1 seek="$offset" conv=notrunc status=none
		echo "repaired $file"
	done <<-EOF
		1917706 222 GFX/ALLPSX2.FLC
		4411282 352 GFX/AAAA.TGA
		5346326 366 GFX/GLENN.TGA
		5969666 242 TABLES/POTTERS/POTTRUS3.EMP
		7045922 036 PINDOS.EXE
	EOF
	check "$out/P_200996_REPAIRED.ZIP" $repaired
	unpack "$out/P_200996_REPAIRED.ZIP" qdup
	rm "$out/P_200996_REPAIRED.ZIP"
fi
