/* libsnd check against the Psy-Q layer: load a VAB over the PC link, key
   one tone the way 360X's ship demo does, then tick once (SS_NOTICK) and
   let it play. check.py measures the emulator's audio output: the note
   lasts as long as the waveform at the pitch the 1996 library computes
   (7D4h for note 65 of 360X's B1_5 bank), and comes out on the left only
   (the bank's program pan is 0), as the 1996 library's voice registers
   show. Run with the 360X tree served over the PC link at its root. */
#include <sys/types.h>
#include <libsn.h>
#include <libetc.h>
#include <libsnd.h>
#include <stdio.h>
#include <stdlib.h>

static u_char *load(char *name)
{
	int fd = PCopen(name, 0, 0), n;
	u_char *p;
	if (fd < 0) { printf("snd: cannot open %s\n", name); return 0; }
	n = PClseek(fd, 0, 2);
	PClseek(fd, 0, 0);
	p = malloc(n);
	PCread(fd, (char *)p, n);
	PCclose(fd);
	return p;
}

int main(void)
{
	u_char *vh, *vb;
	short vab, voice;
	int i;
	ResetCallback();
	PCinit();
	vh = load("SOUND/B1_5.VH");
	vb = load("SOUND/B1_5.VB");
	SsInit();
	SsSetTickMode(SS_NOTICK);
	vab = SsVabOpenHead(vh, -1);
	if (SsVabTransBody(vb, vab) != vab) printf("snd: transfer failed\n");
	SsStart();
	SsSetMVol(127, 127);
	for (i = 0; i < 25; i++) VSync(0);
	voice = SsUtKeyOn(vab, 0, 0, 65, 0, 127, 127);
	printf("snd: keyed voice %d (nothing plays until the tick)\n", voice);
	for (i = 0; i < 50; i++) VSync(0);
	SsSeqCalledTbyT();
	printf("snd: tick\n");
	for (i = 0; i < 600; i++) VSync(0);
	printf("snd: done\n");
	return 0;
}
