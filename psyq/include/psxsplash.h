/* The boot splash, over the PSoXide SDK's psx-splash. Not a Psy-Q library: the
   restoration's way to open both games with the standard studio splash. */
#ifndef _PSXSPLASH_H
#define _PSXSPLASH_H

/* Play the splash with `line` under the studio mark; returns when it has run
   (about 2.5 seconds) or Cross, Circle or Start on the pad given to InitPAD
   (port 1) has ended it. Call it after InitPAD and StartPAD and the video mode
   are set. Takes over the GPU (320 by 240) and leaves it showing a black frame;
   the caller sets the screen up again afterwards. */
void PsxSplash(const char *line);

#endif
