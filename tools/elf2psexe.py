"""Pack a linked MIPS ELF into a PS-X EXE (2048-byte header + flat image)."""
import struct, subprocess, sys, tempfile, os
elf, out = sys.argv[1], sys.argv[2]
objcopy = os.environ.get("OBJCOPY", "mipsel-none-elf-objcopy")
nm = os.environ.get("NM", "mipsel-none-elf-nm")
syms = {}
for line in subprocess.run([nm, elf], capture_output=True, text=True, check=True).stdout.splitlines():
    parts = line.split()
    if len(parts) == 3:
        syms[parts[2]] = int(parts[0], 16)
with tempfile.TemporaryDirectory() as tmp:
    flat = os.path.join(tmp, "flat.bin")
    subprocess.run([objcopy, "-O", "binary", "--remove-section=.sbss", "--remove-section=.bss", elf, flat], check=True)
    body = open(flat, "rb").read()
body += b"\0" * (-len(body) % 2048)
start = 0x80010000
bss = syms["__bss_start"]
header = bytearray(2048)
header[0:8] = b"PS-X EXE"
struct.pack_into("<IIIIIIIIIII", header, 0x10,
                 syms["_start"], syms["_gp"], start, len(body), 0, 0,
                 bss, syms["__bss_end"] - bss, 0x801FFF00, 0, 0)
header[0x4C:0x4C + 23] = b"PSoXide C test program."
open(out, "wb").write(bytes(header) + body)
print(f"{out}: entry {syms['_start']:#x} text {len(body)} bytes bss {syms['__bss_end'] - bss} bytes")
