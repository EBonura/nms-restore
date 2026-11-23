/* PSoXide Psy-Q compatibility layer: the memory card driver calls, on the
   SDK's psx-mc card transport (psyq/src/card.rs); each delivers its event
   before it returns. _CARD is the save file header of the public memory
   card format: "SC", the icon display flag, the block count, a 64-byte
   title, 28 reserved bytes, a 16-colour CLUT and up to three 16x16 4-bit
   icon frames. */
#ifndef PSYQ_CARDIO_H
#define PSYQ_CARDIO_H
#include <sys/types.h>

typedef struct {
	char Magic[2];
	char Type;
	char BlockEntry;
	char Title[64];
	char reserve[28];
	char Clut[32];
	char Icon[3][128];
} _CARD;

long _card_info(long chan);
long _card_load(long chan);
long _card_clear(long chan);
long _card_read(long chan, long block, u_char *buf);
long _card_write(long chan, long block, u_char *buf);
void _new_card(void);
#endif
