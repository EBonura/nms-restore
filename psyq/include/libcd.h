/* PSoXide Psy-Q compatibility layer: libcd.h, reading files from the CD
   (psyq/src/cd.rs). Reads are blocking: CdRead returns once the sectors are
   in memory, so CdReadSync(0, result) returns 0 at once. */
#ifndef PSYQ_LIBCD_H
#define PSYQ_LIBCD_H

/* disc position: BCD minute, second, sector (sector 0 is 00:02:00) */
typedef struct {
	unsigned char minute;
	unsigned char second;
	unsigned char sector;
	unsigned char track;
} CdlLOC;

typedef struct {
	CdlLOC pos;			/* first sector */
	unsigned long size;		/* bytes */
	char name[16];
} CdlFILE;

/* drive commands for CdControl / CdControlB (the drive's command numbers) */
#define CdlNop		0x01
#define CdlSetloc	0x02
#define CdlStop		0x08
#define CdlPause	0x09
#define CdlSetmode	0x0e
#define CdlSeekL	0x15
#define CdlSeekP	0x16

/* mode bits (the drive's Setmode byte) */
#define CdlModeSpeed	0x80	/* double speed */

/* CdSync result */
#define CdlComplete	2

int CdInit(void);
CdlFILE *CdSearchFile(CdlFILE *fp, char *name);
int CdControl(unsigned char com, unsigned char *param, unsigned char *result);
int CdControlB(unsigned char com, unsigned char *param, unsigned char *result);
int CdRead(int sectors, unsigned long *buf, int mode);
int CdReadSync(int mode, unsigned char *result);
int CdSync(int mode, unsigned char *result);
CdlLOC *CdIntToPos(int i, CdlLOC *p);
int CdPosToInt(CdlLOC *p);
unsigned long CdReadyCallback(void (*func)());

/* Not Psy-Q's (psyq/src/loadfail.rs): show that file could not be loaded at
   the loader's step (LOOKUP, READ, MEMORY...) with the drive's side of it,
   and stop there. CdRead and CdSearchFile try a failed read five times, with
   the drive re-synced between tries, before they report it. */
void PsyqLoadFailed(const char *file, const char *step) __attribute__((noreturn));
/* Test builds only (PSYQ_TEST_CD_FAULT=1): the next read waits delay
   vblanks, then its first fails tries fail as a seek that spun out. */
void CdTestFault(int fails, int delay);

#endif
