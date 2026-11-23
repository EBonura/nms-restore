
#ifndef	__H_HIGH
#define	__H_HIGH

/**************************************
*         DECLARE STRUCTURES          *
**************************************/

typedef struct	{				/* RESTORE: was in HIGH.C; LOADSAVE.C saves */
				ULONG	highscore;		/* and loads the table (the Saturn's */
				UBYTE	highname[12];		/* BUPRAM.C does the same) */
				UBYTE	nullstring;
				}HIGH;

extern	HIGH	highest[10];

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	High_Scores(void);
void	Check_High_Score(void);

#endif

