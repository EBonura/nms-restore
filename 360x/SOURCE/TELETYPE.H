
#ifndef	__H_TELETYPE
#define	__H_TELETYPE

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Init_Teletype(void);
void	Print_Teletype(TEXT *text,SHORT lines);

/**************************************
*          DECLARE DEFINES            *
**************************************/

/**************************************
*         DECLARE VARIABLES           *
**************************************/

extern	USHORT	teletypex;
extern	USHORT	teletypey;
extern	USHORT	teletypenumber;
extern	USHORT	teletypepoint;
extern	USHORT	teletypedelay;

#endif

