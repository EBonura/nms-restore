/* PSoXide Psy-Q compatibility layer: the sound library (libsnd) as 360X
   uses it: VAB sound banks (a header the game keeps in RAM and a body of
   ADPCM waveforms sent to sound RAM), one-shot notes from a bank
   (SsUtKeyOn, 360X's sound effects) and SEQ songs, ticked by the game
   (SS_NOTICK: SsSeqCalledTbyT once per vertical blank). psyq/src/snd.rs, on
   psx-spu. Written from how NMS's code uses each name and from the public
   VAB and SEQ file formats. */
#ifndef PSYQ_LIBSND_H
#define PSYQ_LIBSND_H
#include <sys/types.h>

/* bytes of the game's sequence table per track (SsSetTableSize) */
#define SS_SEQ_TABSIZ   176

/* SsSetTickMode: who advances the songs */
#define SS_NOTICK       0x1000  /* the game calls SsSeqCalledTbyT */
#define SS_TICK50       50
#define SS_TICK60       60
#define SS_TICKVSYNC    0x2000  /* every vertical blank */

/* SsSeqPlay */
#define SSPLAY_PAUSE    0
#define SSPLAY_PLAY     1
#define SSPLAY_INFINITY 0       /* loop count: forever */

void  SsInit(void);
void  SsSetTableSize(char *table, short s_max, short t_max);
void  SsSetTickMode(long mode);
void  SsStart(void);
void  SsEnd(void);
void  SsSetMVol(short voll, short volr);
void  SsSetMono(void);
void  SsSetStereo(void);

short SsVabOpenHead(u_char *addr, short vabid);     /* -1 on failure */
short SsVabTransBody(u_char *addr, short vabid);    /* vabid, or -1 */
/* the body in pieces, one call per piece in order: vabid once the whole
   body is in sound RAM, -2 before, -1 on error */
short SsVabTransBodyPartly(u_char *addr, u_long bufsize, short vabid);
void  SsVabClose(short vabid);

short SsSeqOpen(u_long *addr, short vabid);         /* -1 on failure */
void  SsSeqClose(short seq);
void  SsSeqPlay(short seq, char mode, short count);
void  SsSeqStop(short seq);
void  SsSeqSetVol(short seq, short voll, short volr);
void  SsSeqCalledTbyT(void);

/* key a note of program prog, tone tone of a bank; returns the voice
   (0-23) or -1 */
short SsUtKeyOn(short vabid, short prog, short tone, short note, short fine,
                short voll, short volr);
short SsUtKeyOff(short voice, short vabid, short prog, short tone, short note);
short SsUtAllKeyOff(short mode);
#endif
