/* Movie playback, over the PSoXide SDK's psx-fmv player. Not a Psy-Q library:
   the restoration's way to show a film converted to the SDK's STR format. */
#ifndef _PSXFMV_H
#define _PSXFMV_H

/* Bytes of memory FmvPlay needs (15-bit picture). */
unsigned long FmvMemoryBytes(unsigned long width, unsigned long height,
                             unsigned long max_chunks, unsigned long rle_words);

/* Play the movie in the root file `name` (8.3 name, no ";1") from the disc
   until it ends or a button of skip_mask (padequ.h bits) shows in `pad`, a
   ControllerPacket the VBlank handler keeps current. mem: FmvMemoryBytes
   bytes, word aligned, unused meanwhile. Returns 0 ended, 1 skipped, 2 could
   not play. Takes over the drive, MDEC and display; the caller sets the
   screen up again afterwards. */
int FmvPlay(const char *name, unsigned long frames, unsigned long width,
            unsigned long height, unsigned long max_chunks,
            unsigned long rle_words, void *mem, const void *pad,
            unsigned long skip_mask);

#endif
