/* PSoXide Psy-Q compatibility layer: kernel services (events, critical
   sections, the controller and memory card drivers). Event class and spec
   values are the PS1 BIOS's documented ones. psyq/src/kernel.rs, card.rs. */
#ifndef PSYQ_KERNEL_H
#define PSYQ_KERNEL_H
#include <sys/types.h>

/* event classes */
#define RCntCNT0   0xF2000000
#define RCntCNT1   0xF2000001
#define RCntCNT2   0xF2000002
#define RCntCNT3   0xF2000003   /* vertical blank */
#define HwCARD     0xF0000011
#define SwCARD     0xF4000001
/* event specs */
#define EvSpCOUNT  0x0001
#define EvSpINT    0x0002
#define EvSpIOE    0x0004
#define EvSpTIMOUT 0x0100
#define EvSpNEW    0x2000
#define EvSpERROR  0x8000
/* event modes */
#define EvMdINTR   0x1000       /* call the handler from the interrupt */
#define EvMdNOINTR 0x2000       /* only mark the event */

long OpenEvent(unsigned long cls, long spec, long mode, void (*func)());
long CloseEvent(long event);
long EnableEvent(long event);
long DisableEvent(long event);
long TestEvent(long event);
long WaitEvent(long event);
void EnterCriticalSection(void);
void ExitCriticalSection(void);

/* controller driver: fills the two buffers at every vertical blank */
void InitPAD(void *buf1, long len1, void *buf2, long len2);
long StartPAD(void);
void StopPAD(void);
void ChangeClearPAD(long val);

/* memory card: nothing to set up; card.rs talks to the card per call */
void InitCARD(long pad_enable);
long StartCARD(void);
long StopCARD(void);
void _bu_init(void);

/* the BIOS file system on memory cards ("bu00:" port 1, "bu10:" port 2):
   the card's standard directory, open with O_CREAT|(blocks<<16) makes a
   file, names match with '?' and '*' (psyq/src/card.rs) */
int  open(char *name, int mode);
int  close(int fd);
int  read(int fd, void *buf, int n);
int  write(int fd, void *buf, int n);
int  format(char *dev);
int  delete(char *name);
#endif
