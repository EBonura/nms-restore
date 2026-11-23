/* PSoXide Psy-Q compatibility layer: vertical blank, callbacks, video mode.
   psyq/src/kernel.rs and gpu.rs. */
#ifndef PSYQ_LIBETC_H
#define PSYQ_LIBETC_H
#include <sys/types.h>

#define MODE_NTSC 0
#define MODE_PAL  1

/* VSync(0) waits for the next vertical blank, VSync(n) for the n-th since
   the previous call, VSync(1) only reads, VSync(-1) returns the number of
   vertical blanks since ResetCallback. */
int  VSync(int mode);
/* not Psy-Q's: the next vertical blank whose count has the parity given,
   then on past the blank (kernel.rs; QD Up's 480-line screens) */
int  VSyncField(int parity);
int  VSyncCallback(void (*func)(void));
int  ResetCallback(void);
long SetVideoMode(long mode);   /* returns the previous mode */
long GetVideoMode(void);

/* root counters: QD Up only starts one; nothing is counted yet */
long StartRCnt(unsigned long spec);
long StopRCnt(unsigned long spec);
/* PadRead()'s button bits: the controller's two button bytes, inverted,
   the first byte (select, L3, R3, start, up, right, down, left) in the high
   half and the second (L2, R2, L1, R1, triangle, circle, cross, square) in
   the low one. 360X names them in TITLE.C's cube demo, which it never
   runs; PadRead itself is not implemented. */
#define PADLup     (1 << 12)
#define PADLright  (1 << 13)
#define PADLdown   (1 << 14)
#define PADLleft   (1 << 15)
#define PADk       (1 << 8)     /* select */
#define PADh       (1 << 11)    /* start */
#define PADstart   PADh
#define PADselect  PADk
#define PADo       (1 << 0)     /* L2 */
#define PADm       (1 << 1)     /* R2 */
#define PADn       (1 << 2)     /* L1 */
#define PADl       (1 << 3)     /* R1 */
#define PADRup     (1 << 4)
#define PADRright  (1 << 5)
#define PADRdown   (1 << 6)
#define PADRleft   (1 << 7)
#endif
