#!/usr/bin/env python3
"""Make 360X's intro film for the SDK's FMV player.

    x360_film.py DATA_ROOT OUT.STR OUT.h

DATA_ROOT is the 1996 tree (the unpacked 360X.ZIP). The film is NMS's
GRAPHICS/FLICS/SAT2.FLC (320x240, 191 frames, 71 ms a frame: the animation NMS
made for the Saturn's intro, whose call is commented out in the Saturn main).
ffmpeg decodes it (every frame, at the file's own 1000/71 frame rate, no rate
conversion) to a Y4M, and the PSoXide SDK's psx-str-cook encodes that as a
video-only 2x STR with bitstream v2 video, as raw 2336-byte sectors (the disc
build adds the file with mkisopsx --xa-file). Name the encoder with the
PSX_STR_COOK environment variable (360x/disc.sh builds it from the pinned SDK
checkout and sets it). The picture is NMS's; the encoding is ours.

OUT.h holds what psx-fmv's player needs to know about the movie, measured from
the STR's own headers (the layout is in psx-fmv's stream.rs): frames, size,
the most sectors a frame spans and the most run-length words a frame decodes to
(the BS header's word count, which the SDK's encoder test checks is exact).
A cook record, OUT.STR.cook.txt, names the encoder, the SDK checkout's commit
and the sha256 of every input and output.
"""
import hashlib
import os
import struct
import subprocess
import sys


def find_str_cook():
    p = os.environ.get("PSX_STR_COOK")
    if not p or not os.access(p, os.X_OK):
        sys.exit("x360_film.py: psx-str-cook not found. It is the PSoXide SDK's host movie encoder "
                 "(crates/psx-str-cook, cargo build --release -p psx-str-cook in the SDK checkout the "
                 "build pins). Set PSX_STR_COOK=/path/to/psx-str-cook (360x/disc.sh does).")
    return p


def sha256(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def measure(data):
    """frames, width, height, max chunks per frame, max MDEC words per frame."""
    assert len(data) % 2336 == 0
    frames = set()
    w = h = maxchunks = maxwords = 0
    for i in range(0, len(data), 2336):
        s = data[i + 8:i + 8 + 2048]
        magic, kind, idx, count, frame, size, sw, sh = struct.unpack_from("<HHHHIIHH", s, 0)
        if magic != 0x0160 or kind != 0x8001:
            continue
        frames.add(frame)
        w, h = sw, sh
        maxchunks = max(maxchunks, count)
        maxwords = max(maxwords, struct.unpack_from("<H", s, 20)[0])
    return len(frames), w, h, maxchunks, maxwords


def main(argv):
    if len(argv) != 3:
        sys.exit(__doc__)
    root, out_str, out_h = argv
    flc = os.path.join(root, "GRAPHICS/FLICS/SAT2.FLC")
    cook = find_str_cook()
    y4m = out_str + ".y4m"
    subprocess.run(["ffmpeg", "-v", "error", "-y", "-i", flc, "-fps_mode", "passthrough",
                    "-pix_fmt", "yuv420p", "-f", "yuv4mpegpipe", y4m], check=True)
    subprocess.run([cook, "encode", "--video", y4m, "--out", out_str, "--size", "320x240",
                    "--speed", "2"], check=True)
    with open(out_str, "rb") as f:
        frames, w, h, chunks, words = measure(f.read())
    with open(out_h, "w") as f:
        f.write("/* made by tools/x360_film.py from SAT2.FLC; do not edit */\n"
                f"#define FILM_FRAMES {frames}\n#define FILM_WIDTH {w}\n#define FILM_HEIGHT {h}\n"
                f"#define FILM_MAX_CHUNKS {chunks}\n#define FILM_RLE_WORDS {words}\n")
    sdk = os.environ.get("PSX_STR_COOK_SDK_COMMIT", "unknown")
    ff = subprocess.run(["ffmpeg", "-version"], capture_output=True, text=True).stdout.splitlines()[0]
    with open(out_str + ".cook.txt", "w") as f:
        f.write(f"encoder psx-str-cook sha256 {sha256(cook)}\nSDK commit {sdk}\n{ff}\n"
                f"input SAT2.FLC sha256 {sha256(flc)}\ndecoded y4m sha256 {sha256(y4m)}\n"
                f"output {os.path.basename(out_str)} sha256 {sha256(out_str)}\n"
                f"frames {frames} {w}x{h} max chunks {chunks} max RLE words {words}\n")
    os.remove(y4m)
    print(f"{out_str}: {frames} frames {w}x{h}, up to {chunks} sectors and {words} MDEC words a frame")


if __name__ == "__main__":
    main(sys.argv[1:])
