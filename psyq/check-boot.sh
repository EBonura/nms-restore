#!/bin/sh
# Check a program linked with boot-only memory (psyq-boot.ld): the heap takes that memory back after
# boot, so nothing outside it may call into it except the program's entry points.
#
#   psyq/check-boot.sh ELF ENTRY...     ENTRY: the symbols the program calls (FmvPlay, PsxSplash, ...)
#
# Every direct call (jal, j) in the image is checked; a pointer to a boot-only function is not (the
# layer takes none). Prints the offending instructions and exits 1.
set -e
elf=$1; shift
sym() { mipsel-none-elf-nm "$elf" | awk -v s="$1" '$3==s {print "0x" $1}'; }
bs=$(sym __boot_start); be=$(sym __boot_load_end)
entries=""
for e in "$@"; do entries="$entries $(sym "$e")"; done
mipsel-none-elf-objdump -d --no-show-raw-insn "$elf" | awk -v bs="$bs" -v be="$be" -v entries="$entries" '
	function h(s,   i, v) { v = 0; s = tolower(s); sub(/^0x/, "", s); for (i = 1; i <= length(s); i++) v = v * 16 + index("0123456789abcdef", substr(s, i, 1)) - 1; return v }
	BEGIN { bs = h(bs); be = h(be); n = split(entries, e, " "); for (i = 1; i <= n; i++) ok[h(e[i])] = 1 }
	{ sub(/:$/, "", $1); pc = h($1) }
	$2 == "jal" || $2 == "j" {
		t = h($3)
		if (t >= bs && t < be && !(pc >= bs && pc < be) && !(t in ok)) { print "boot-only memory is called from outside it: " $0; bad = 1 }
	}
	END { exit bad }'
