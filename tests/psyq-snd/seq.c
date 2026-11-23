/* SEQ playback check against the Psy-Q layer: plays OPTPA.SEQ on the
   OPTIONS bank exactly as NMS's March 1996 build of 360X starts it
   (NMS.zip NMS/360WORK, SOUND.C InitialiseSound and MAIN.C: both songs opened,
   the bank opened and sent, main volume 127, OPTPA played for ever, ticked
   from the vertical blank callback on a PAL display). check_seq.py runs
   that 1996 build and this one in the emulator and compares their audio.
   Run with the 360X tree served over the PC link at its root. */
#include <sys/types.h>
#include <libsn.h>
#include <libetc.h>
#include <libgpu.h>
#include <libsnd.h>
#include <stdio.h>
#include <stdlib.h>

static u_char *load(char *name)
{
	int fd = PCopen(name, 0, 0), n;
	u_char *p;
	if (fd < 0) { printf("seq: cannot open %s\n", name); return 0; }
	n = PClseek(fd, 0, 2);
	PClseek(fd, 0, 0);
	p = malloc(n);
	PCread(fd, (char *)p, n);
	PCclose(fd);
	return p;
}

static volatile int vbl;
static void tick(void) { vbl++; SsSeqCalledTbyT(); }

int main(void)
{
	u_char *vh, *vb, *sa, *sb;
	short vab = 0, seq, seq2;
	DISPENV d;
	char table[SS_SEQ_TABSIZ * 2 * 2];
	ResetCallback();
	PCinit();
	SetVideoMode(MODE_PAL);
	ResetGraph(0);
	SetDefDispEnv(&d, 0, 0, 320, 256);
	PutDispEnv(&d);
	SsInit();
	vh = load("SOUND/OPTIONS.VH");
	vb = load("SOUND/OPTIONS.VB");
	sa = load("SOUND/OPTPA.SEQ");
	sb = load("SOUND/OPTPB.SEQ");
	VSyncCallback(tick);
	/* InitialiseSound, March 1996 */
	SsInit();
	SsSetTableSize(table, 2, 2);
	SsSetTickMode(SS_NOTICK);
	seq = SsSeqOpen((u_long *)sa, vab);
	seq2 = SsSeqOpen((u_long *)sb, vab);
	vab = SsVabOpenHead(vh, -1);
	if (SsVabTransBody(vb, vab) != vab) printf("seq: transfer failed\n");
	SsStart();
	SsSetMVol(127, 127);
	SsSeqSetVol(seq, 127, 127);
	SsSeqSetVol(seq2, 127, 127);
	SsSeqPlay(seq, SSPLAY_PLAY, SSPLAY_INFINITY);
	printf("seq: playing (songs %d %d, bank %d)\n", seq, seq2, vab);
	while (vbl < 50 * 50) VSync(0);
	printf("seq: done after %d vertical blanks\n", vbl);
	return 0;
}
